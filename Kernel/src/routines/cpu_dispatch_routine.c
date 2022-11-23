#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "blocked_routine.h"
#include "cpu_dispatch_routine.h"
#include "memoria_routine.h"
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
        {   /////// WAITING FOR CPU ///////
            codigo_operacion = recibir_operacion(socket);
            (void) largo_paquete(socket);
        }

        /////// DEALING WITH CASES ///////
        t_pcb* unPcb = NULL;
        char* dispositivo = NULL;
        uint32_t* unidades = NULL, *seg_num = NULL, *page_num = NULL;
        switch (codigo_operacion)
        {
        case INIT_CPU:
            break;
        case DESALOJO_PROCESO:
            {
                unPcb = obtener_y_actualizar_pcb_recibido(socket);
                sale_de_exec(unPcb, DESALOJO_PROCESO);

                {   ////////////// LOGGEANDO //////////////
                    pthread_mutex_lock(&mutex_logger);
                    log_info(logger_dispatch, "Proceso %lu : EXEC -> READY (DESALOJO)", (unsigned long)(unPcb->id));
                    pthread_mutex_unlock(&mutex_logger);
                }

                ingresar_a_ready(unPcb, DESALOJO_PROCESO);
                unPcb = NULL;
                sem_post(&sem_proceso_entro_a_ready);
            }
            break;
        case EXIT_PROCESO:
            {
                unPcb = obtener_y_actualizar_pcb_recibido(socket);
                sale_de_exec(unPcb, EXIT_PROCESO);

                {   ////////////// LOGGEANDO //////////////
                    pthread_mutex_lock(&mutex_logger);
                    log_info(logger_dispatch, "Proceso %lu : EXEC -> EXIT (EXIT)", (unsigned long)(unPcb->id));
                    pthread_mutex_unlock(&mutex_logger);
                }
                
                finalizar_proceso(unPcb);
            }
            break;
        case BLOQUEO_PROCESO:
            {
                unPcb = obtener_y_actualizar_pcb_recibido(socket);
                sale_de_exec(unPcb, BLOQUEO_PROCESO);

                {   ////////////// LOGGEANDO //////////////
                    pthread_mutex_lock(&mutex_logger);
                    log_info(logger_dispatch, "Proceso %lu : EXEC -> BLOCKED (BLOQUEO)", (unsigned long)(unPcb->id));
                    pthread_mutex_unlock(&mutex_logger);
                }

                {   ////////////// BLOCKEANDO //////////////
                    dispositivo = (char*)recibir(socket);
                    unidades = (uint32_t*)recibir(socket);

                    bloquear_proceso(unPcb, dispositivo, *unidades);

                    free(dispositivo);
                    free(unidades);
                    dispositivo = NULL;
                    unidades = NULL;
                }
            }
            break;
        case PAGE_FAULT:
            {
                unPcb = obtener_y_actualizar_pcb_recibido(socket);
                sale_de_exec(unPcb, PAGE_FAULT);

                {   ////////////// LOGGEANDO //////////////
                    pthread_mutex_lock(&mutex_logger);
                    log_info(logger_dispatch, "Proceso %lu : EXEC -> BLOCKED (PAGE_FAULT)", (unsigned long)(unPcb->id));
                    pthread_mutex_unlock(&mutex_logger);
                }

                {   ////////////// BLOCKEANDO //////////////
                    seg_num = (uint32_t*)recibir(socket);
                    page_num = (uint32_t*)recibir(socket);

                    page_fault_process(unPcb, *seg_num, *page_num);
                    
                    free(seg_num);
                    free(page_num);
                    seg_num = NULL;
                    page_num = NULL;
                }
            }
            break;
        case SEG_FAULT:
            {
                unPcb = obtener_y_actualizar_pcb_recibido(socket);
                sale_de_exec(unPcb, SEG_FAULT);

                {   ////////////// LOGGEANDO //////////////
                    pthread_mutex_lock(&mutex_logger);
                    log_info(logger_dispatch, "Proceso %lu : EXEC -> EXIT (SEG_FAULT))", (unsigned long)(unPcb->id));
                    pthread_mutex_unlock(&mutex_logger);
                }
                
                finalizar_proceso(unPcb);
            }
            break;
        default:
            exit(EXIT_FAILURE);
            break;
        }
        give_cpu_next_pcb(socket);
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
        agregar_a_paquete(paquete, (void *)&(unPcb->cant_segmentos), sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)(unPcb->segmentos), sizeof(t_segmento_pcb) * (unPcb->cant_segmentos));
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
        log_info(logger_dispatch, "Proceso %lu : READY -> EXEC", (unsigned long)(unPcb->id));
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
    }
    pthread_mutex_unlock(&mutex_pcb_list);

    return unPcb;
}

void finalizar_proceso(t_pcb* unPcb)
{
    unPcb -> pipeline.operacion=EXIT;
    unPcb -> pipeline.valor=0;
    sem_post(&(unPcb -> console_semaphore));
    sem_post(&sem_grado_multiprogramacion);
}
