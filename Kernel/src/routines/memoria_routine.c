#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include "memoria_routine.h"

op_code global_memory_operation;
uint32_t global_seg_to_page_fault;
uint32_t global_page_to_page_fault;
t_pcb *global_pcb_to_memory;

t_log *logger_memoria;

void handshakeMemoria(int);
void enviar_a_memoria(int socket);
void respuesta_memoria(int socket);
void *page_fault_routine(void *param);

void *memoria_routine(void *config)
{
    int socket;

    { ////////////// CONFIG //////////////
        char *ipMemoria = config_get_string_value((t_config *)config, "IP_MEMORIA");
        char *puertoMemoria = config_get_string_value((t_config *)config, "PUERTO_MEMORIA");
        socket = crear_conexion(ipMemoria, puertoMemoria);
        logger_memoria = log_create("../kernel.log", "Kernel - Memoria", false, LOG_LEVEL_INFO);
    }

    handshakeMemoria(socket);

    while (1)
    {
        sem_wait(&sem_memory_routine);

        enviar_a_memoria(socket);

        respuesta_memoria(socket);

        sem_post(&sem_memory_operation_resolved);
    }

    return NULL;
}

void handshakeMemoria(int socket)
{
    { ////////////// LOGGEANDO //////////////
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_memoria, "Conectando a memoria");
        pthread_mutex_unlock(&mutex_logger);
    }
    t_paquete *paquete = crear_paquete(INIT_KERNEL);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);

    op_code code = recibir_operacion(socket);
    int __attribute__((unused)) tamaño_paquete = largo_paquete(socket);
    if (code != INIT_MEMORIA)
    {
        { ////////////// LOGGEANDO //////////////
            pthread_mutex_lock(&mutex_logger);
            log_info(logger_memoria, "Fallo conexion a memoria");
            pthread_mutex_unlock(&mutex_logger);
        }
        exit(EXIT_FAILURE);
    }
}

void enviar_a_memoria(int socket)
{
    t_paquete *paquete = NULL;
    switch (global_memory_operation)
    {
    case NUEVO_PROCESO:
        paquete = crear_paquete(NUEVO_PROCESO);
        agregar_a_paquete(paquete, (void *)&(global_pcb_to_memory->id), sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)&(global_pcb_to_memory->cant_segmentos), sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)(global_pcb_to_memory->segmentos), sizeof(t_segmento_pcb) * (global_pcb_to_memory->cant_segmentos));
        break;
    case PAGE_FAULT:
        paquete = crear_paquete(PAGE_FAULT);
        agregar_a_paquete(paquete, (void *)&(global_pcb_to_memory->id), sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)&((global_pcb_to_memory->segmentos)[global_seg_to_page_fault]).identificador_tabla, sizeof(uint32_t));
        agregar_a_paquete(paquete, (void *)&global_page_to_page_fault, sizeof(uint32_t));
        break;
    case EXIT_PROCESO:
    case SEG_FAULT:
        paquete = crear_paquete(EXIT_PROCESO);
        agregar_a_paquete(paquete, (void *)&(global_pcb_to_memory->id), sizeof(uint32_t));
    default:
        { ////////////// LOGGEANDO //////////////
            pthread_mutex_lock(&mutex_logger);
            log_info(logger_memoria, "Error en memoria_routine, operacion a enviar desconocida.");
            pthread_mutex_unlock(&mutex_logger);
        }
        exit(EXIT_FAILURE);
        break;
    }
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void respuesta_memoria(int socket)
{
    uint32_t* pid = NULL;
    op_code operacion = recibir_operacion(socket);
    (void)largo_paquete(socket);
    switch (operacion)
    {
    case NUEVO_PROCESO:
        pid = (uint32_t*)recibir(socket);
        if((*pid) != global_pcb_to_memory->id)
        {
            { ////////////// LOGGEANDO //////////////
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_memoria, "Error en memoria_routine: PID %u != PID %u.", *pid, global_pcb_to_memory->id);
                pthread_mutex_unlock(&mutex_logger);
            }   
            exit(EXIT_FAILURE);
        }
        
        free(global_pcb_to_memory->segmentos);
        global_pcb_to_memory->segmentos=(t_segmento_pcb*)recibir(socket);
        
        break;
    case PAGE_FAULT:
    case EXIT_PROCESO:
        break;
    default:
        { ////////////// LOGGEANDO //////////////
            pthread_mutex_lock(&mutex_logger);
            log_info(logger_memoria, "Error en memoria_routine, operacion recibida desconocida.");
            pthread_mutex_unlock(&mutex_logger);
        }
        exit(EXIT_FAILURE);
        break;
    }
}

void page_fault_process(t_pcb *pcb, uint32_t seg_num, uint32_t page_num)
{
    void *param;
    {
        param = malloc(sizeof(uint32_t) * 2);
        *(t_pcb **)param = pcb;
        *(uint32_t *)(param + sizeof(t_pcb *)) = seg_num;
        *(uint32_t *)(param + sizeof(t_pcb *) + sizeof(uint32_t)) = page_num;
    }
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, page_fault_routine, param);
}

void *page_fault_routine(void *param)
{
    t_pcb *pcb = *(t_pcb **)param;
    uint32_t seg_num = *(uint32_t *)(param + sizeof(t_pcb *));
    uint32_t page_num = *(uint32_t *)(param + sizeof(t_pcb *) + sizeof(uint32_t));

    sem_wait(&sem_memory_handlers);
    global_memory_operation = PAGE_FAULT;
    global_pcb_to_memory = pcb;
    global_seg_to_page_fault = seg_num;
    global_page_to_page_fault = page_num;

    sem_post(&sem_memory_routine);

    sem_wait(&sem_memory_operation_resolved);

    sem_post(&sem_memory_handlers);

    ingresar_a_ready(pcb, BLOQUEO_PROCESO);
    sem_post(&sem_proceso_entro_a_ready);

    free(param);
    return NULL;
}