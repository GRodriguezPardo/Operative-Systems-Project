#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include <unistd.h>
#include "consola_routine.h"
#include "planificador_routine.h"
#include "../globals.h"
#include "../kernel_utils.h"

void *consola_routine(void *param)
{
    ////////////// DECODING PARAM //////////////
    int socket_cliente, consola_id;
    {
        socket_cliente = ((int *)param)[0];
        consola_id = ((int *)param)[1];
    }

    ////////////// CREANDO LOGGER DEL HANDLER //////////////
    t_log *logger;
    {
        char buffer[25];
        sprintf(buffer, "Kernel - Console %d", consola_id);
        logger = log_create("../kernel.log", buffer, 0, LOG_LEVEL_INFO);
    }

    ////////////// RECIBIENDO OPERACION //////////////
    int __attribute__((unused)) tamanio_paquete, tamanio_restante;
    {
        op_code codigo_operacion = recibir_operacion(socket_cliente);
        tamanio_paquete = tamanio_restante = largo_paquete(socket_cliente);

        if (codigo_operacion != NUEVO_PROCESO)
        {
            perror("Error: Inicio de consola con codigo inesperado. Finalizando hilo.\n");
            logger_monitor_error(logger, "Error: Inicio de consola con codigo inesperado. Finalizando hilo.");
            exit(EXIT_FAILURE);
        }
    }

    ////////////// RECIBIENDO SEGMENTOS //////////////
    uint32_t segmentos[4];
    {
        void *msg = NULL;

        for (size_t i = 0; i < 4; i++)
        {
            msg = recibir(socket_cliente);
            segmentos[i] = *((uint32_t *)msg);
            free(msg);
            msg = NULL;
        }
        tamanio_restante -= 32;
    }

    ////////////// RECIBIENDO INSTRUCCIONES //////////////
    char **instrucciones;
    uint32_t cantidad = 0;
    {
        void *msg = NULL;

        while (tamanio_restante > 0)
        {
            msg = recibir(socket_cliente);
            instrucciones = (char **)realloc(instrucciones, sizeof(char *) * (cantidad + 1));
            instrucciones[cantidad] = msg;
            msg = NULL;

            tamanio_restante -= (strlen(instrucciones[cantidad]) + 1 + sizeof(int));
            cantidad += 1;
        }
    }

    ////////////// CREANDO PCB //////////////
    t_pcb *mi_pcb = (t_pcb *)malloc(sizeof(t_pcb));
    {
        mi_pcb->id = consola_id;
        mi_pcb->cant_instrucciones = cantidad;
        mi_pcb->instrucciones = instrucciones;
        mi_pcb->program_counter = 0;
        sem_init(&(mi_pcb->console_semaphore), 0, 0);
        sem_init(&(mi_pcb->console_waiter_semaphore), 0, 0);
        mi_pcb->pipeline.operacion = 0;
        mi_pcb->pipeline.valor = 0;

        for (size_t i = 0; i < 4; i++)
        {
            mi_pcb->segmentos[i].tamanio = segmentos[i];
            mi_pcb->segmentos[i].identificador_tabla = 0;
        }
    }

    ////////////// ENTRANDO EN COLA DEL PLANIFICADOR DE LARGO PLAZO //////////////
    logger_monitor_info(logger, "Ingresando PCB a NEW.");
    ingresar_a_new(mi_pcb);

    ////////////// QUEDANDO A LA ESPERA DE INSTRUCCIONES //////////////
    {
        t_paquete *paquete;
        uint32_t *input = NULL;
        int __attribute__((unused)) tamanio_paquete;

        logger_monitor_info(logger, "Quedando a la espera de I/O.");

        while (1)
        {
            sem_wait(&(mi_pcb->console_semaphore));

            switch (mi_pcb->pipeline.operacion)
            {
            case CONSOLE_INPUT:
                logger_monitor_info(logger, "Realizando console input.");

                paquete = crear_paquete(CONSOLE_INPUT);
                enviar_paquete(paquete, socket_cliente);
                eliminar_paquete(paquete);

                if (recibir_operacion(socket_cliente) != CONSOLE_INPUT_RESPUESTA)
                {
                    perror("Error: Fallo al recibir respuesta de console input.\n");
                    logger_monitor_error(logger, "Error: Fallo al recibir respuesta de console input.");
                    exit(EXIT_FAILURE);
                }
                tamanio_paquete = largo_paquete(socket_cliente);
                input = (uint32_t *)recibir(socket_cliente);
                mi_pcb->pipeline.operacion = CONSOLE_INPUT_RESPUESTA;
                mi_pcb->pipeline.valor = *input;
                free(input);
                input = NULL;

                sem_post(&(mi_pcb -> console_waiter_semaphore));
                break;

            case CONSOLE_OUTPUT:
                logger_monitor_info(logger, "Realizando console output.");

                paquete = crear_paquete(CONSOLE_OUTPUT);
                agregar_a_paquete(paquete, (void *)&(mi_pcb->pipeline.valor), sizeof(uint32_t));
                enviar_paquete(paquete, socket_cliente);
                eliminar_paquete(paquete);

                if (recibir_operacion(socket_cliente) != CONSOLE_OUTPUT_RESPUESTA)
                {
                    perror("Error: Fallo al recibir respuesta de console input.\n");
                    logger_monitor_error(logger, "Error: Fallo al recibir respuesta de console input.");
                    exit(EXIT_FAILURE);
                }
                tamanio_paquete = largo_paquete(socket_cliente);

                mi_pcb->pipeline.operacion = CONSOLE_OUTPUT_RESPUESTA;
                sem_post(&(mi_pcb -> console_waiter_semaphore));
                break;
            case EXIT:
                logger_monitor_info(logger, "Finalizando.");
                pthread_mutex_lock(&mutex_pcb_list);
                {
                    search_for_id_buffer = mi_pcb ->id;
                    list_remove_by_condition(pcb_list, search_for_id);
                    pcb_element_destroyer((void*) mi_pcb);
                }
                pthread_mutex_unlock(&mutex_pcb_list);

                paquete = crear_paquete(EXIT);
                enviar_paquete(paquete, socket_cliente);
                eliminar_paquete(paquete);
                
                return NULL;
            default:
                perror("Error: Fallo al recibir operacion de IO.\n");
                logger_monitor_error(logger, "Error: Fallo al recibir operacion de IO.");
                exit(EXIT_FAILURE);
                break;
            }
        }
    }
    return NULL;
}
