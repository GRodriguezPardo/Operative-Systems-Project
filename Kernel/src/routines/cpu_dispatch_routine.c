#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "cpu_dispatch_routine.h"
#include "blocked_routine.h"
#include "../globals.h"
#include "../kernel_utils.h"

t_log* logger_dispatch;

void *cpu_dispatch_routine(void *config)
{
    int socket;
    {   ////////////// CONFIG //////////////
        char *puertoServidor, *ipCPU;
        puertoServidor = config_get_string_value((t_config *)config, "PUERTO_CPU_DISPATCH");
        ipCPU = config_get_string_value((t_config *)config, "IP_CPU");
        socket = crear_conexion(ipCPU, puertoServidor);
        logger_dispatch = log_create("../kernel.log", "Kernel - Dispatch", false, LOG_LEVEL_INFO);
    }
    
    while (1)
    {   ////////////// BEGINNING CYCLE //////////////
        op_code codigo_operacion;
        int __attribute__((unused)) tamaño_paquete;
        {   /////// WAITING FOR CPU ///////
            codigo_operacion = recibir_operacion(socket);
            tamaño_paquete = largo_paquete(socket);
        }

        /////// DEALING WITH CASES ///////
        t_pcb* unPcb = NULL;
        char* dispositivo = NULL;
        uint32_t* unidades = NULL;
        switch (codigo_operacion)
        {
        case INIT_CPU:
            give_cpu_next_pcb(socket);
            logger_monitor_info(logger_dispatch, "Inicializando CPU");
            break;
        case DESALOJO_PROCESO:
            unPcb = obtener_y_actualizar_pcb_recibido(socket);
            sale_de_exec(unPcb, DESALOJO_PROCESO);
            ingresar_a_ready(unPcb, DESALOJO_PROCESO);
            unPcb = NULL;
            sem_post(&sem_proceso_entro_a_ready);
            
            give_cpu_next_pcb(socket);
            break;
        case EXIT_PROCESO:
            unPcb = obtener_y_actualizar_pcb_recibido(socket);
            sale_de_exec(unPcb, EXIT_PROCESO);
            finalizar_proceso(unPcb);

            give_cpu_next_pcb(socket);
            break;
        case BLOQUEO_PROCESO:
            unPcb = obtener_y_actualizar_pcb_recibido(socket);
            sale_de_exec(unPcb, BLOQUEO_PROCESO);
            {
                dispositivo = (char*)recibir(socket);
                unidades = (uint32_t*)recibir(socket);

                bloquear_proceso(unPcb, dispositivo, *unidades);

                pthread_mutex_lock(&mutex_logger);
                log_info(logger_dispatch, "Bloqueo proceso %u - %s - %u", unPcb->id, dispositivo, *unidades);
                pthread_mutex_unlock(&mutex_logger);

                free(dispositivo);
                free(unidades);
                dispositivo = NULL;
                unidades = NULL;
            }

            give_cpu_next_pcb(socket);
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }
    }
    return NULL;
}

void give_cpu_next_pcb(int socket)
{
    sem_wait(&sem_proceso_entro_a_ready);
    t_pcb *unPcb = NULL;
    {   ////////////// OBTENIENDO PROXIMO PCB //////////////
        unPcb = obtener_siguiente_a_exec();
    }

    t_paquete *paquete = NULL;
    {   ////////////// ARMANDO PAQUETE //////////////
        paquete = crear_paquete(PROXIMO_PCB);
        agregar_a_paquete(paquete, (void *)&(unPcb->id), sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)&(unPcb->program_counter), sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)&(unPcb->registros), sizeof(uint32_t) * 4);
        agregar_a_paquete(paquete, (void *)&(unPcb->segmentos), sizeof(t_segmento_pcb) * 4);
        agregar_a_paquete(paquete, (void *)&(unPcb->cant_instrucciones), sizeof(uint32_t));

        for (size_t i = 0; i < unPcb->cant_instrucciones; i++)
        {
            agregar_a_paquete(paquete, (void *)(unPcb->instrucciones[i]), strlen(unPcb->instrucciones[i]) + 1);
        }
    }

    
    {   ////////////// ENVIANDO PAQUETE Y LIMPIANDO //////////////
        enviar_paquete(paquete, socket);
        eliminar_paquete(paquete);
    }

    {   ////////////// LOGGEANDO //////////////
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_dispatch, "Enviando al proceso %lu a ejecutar", (unsigned long)(unPcb->id));
        pthread_mutex_unlock(&mutex_logger);
    }
}

t_pcb *obtener_y_actualizar_pcb_recibido(int socket)
{
    uint32_t *id = NULL;
    {   ////////////// OTBENIENDO ID //////////////
        id = recibir(socket);
    }

    t_pcb *unPcb = NULL;
    pthread_mutex_lock(&mutex_pcb_list);
    {   ////////////// ACTUALIZANDO PCB EN LISTA DE PCB //////////////

        {   /////// OBTENIENDO PCB ///////
            search_for_id_buffer = *id;
            unPcb = (t_pcb *)list_find(pcb_list, search_for_id);
            search_for_id_buffer = 0;
        }
        free(id);

        {   /////// ACTUALIZANDO PROGRAM COUNTER ///////
            uint32_t *pc;
            pc = recibir(socket);
            unPcb->program_counter = *pc;
            free(pc);
        }

        
        {   /////// ACTUALIZANDO REGISTROS ///////
            uint32_t *regs;
            regs = recibir(socket);
            for (size_t i = 0; i < 4; i++)
            {
                unPcb->registros[i] = regs[i];
            }
            free(regs);
        }

        
        {   /////// ACTUALIZANDO SEGMENTOS ///////
            t_segmento_pcb *segmentos = NULL;
            segmentos = (t_segmento_pcb *)recibir(socket);
            for (size_t i = 0; i < 4; i++)
            {
                unPcb->segmentos[i] = segmentos[i];
            }
        }
    }
    pthread_mutex_unlock(&mutex_pcb_list);

    {   ////////////// LOGGEANDO //////////////
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_dispatch, "El proceso %lu salio de ejecucion", (unsigned long)(unPcb->id));
        pthread_mutex_unlock(&mutex_logger);
    }

    return unPcb;
}

void finalizar_proceso(t_pcb* unPcb)
{
    unPcb -> pipeline.operacion=EXIT;
    unPcb -> pipeline.valor=0;
    sem_post(&(unPcb -> console_semaphore));
    sem_post(&sem_grado_multiprogramacion);

    {   ////////////// LOGGEANDO //////////////
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_dispatch, "El proceso %lu ha llegado a exit", (unsigned long)(unPcb->id));
        pthread_mutex_unlock(&mutex_logger);
    }
}
