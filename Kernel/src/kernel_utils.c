#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algorithms/fifo.h"
#include "algorithms/round_robin.h"
#include "algorithms/feedback.h"
#include "kernel_utils.h"
#include "globals.h"

void init_globals_kernel(int grado_multiprogramacion)
{
    logger_largo_plazo = log_create("../kernel.log", "Kernel - Planif. Largo Plazo", 0, LOG_LEVEL_INFO);
    logger_blockeos = log_create("../kernel.log", "Kernel - Admin. Blockeos", 0, LOG_LEVEL_INFO);

    sem_init(&sem_grado_multiprogramacion, 0, grado_multiprogramacion);
    sem_init(&sem_proceso_entro_a_new, 0, 0);
    sem_init(&sem_proceso_entro_a_ready, 0, 0);
    sem_init(&sem_interrupt_algorithms, 0, 1);
    sem_init(&sem_interrupt_routine, 0, 0);
    sem_init(&sem_memory_handlers, 0, 1);
    sem_init(&sem_memory_routine, 0, 0);
    sem_init(&sem_memory_operation_resolved, 0, 0);

    pthread_mutex_init(&mutex_logger, NULL);
    pthread_mutex_init(&mutex_pcb_list, NULL);
    pthread_mutex_init(&mutex_cola_new, NULL);

    pcb_list = list_create();
    cola_estado_new = queue_create();
}

void finalizar_kernel(t_config *config, t_log *logger)
{
    finalizar_algoritmo();

    sem_close(&sem_grado_multiprogramacion);
    sem_close(&sem_proceso_entro_a_new);
    sem_close(&sem_proceso_entro_a_ready);
    sem_close(&sem_interrupt_algorithms);
    sem_close(&sem_interrupt_routine);
    sem_close(&sem_memory_handlers);
    sem_close(&sem_memory_routine);
    sem_close(&sem_memory_operation_resolved);
    sem_destroy(&sem_grado_multiprogramacion);
    sem_destroy(&sem_proceso_entro_a_new);
    sem_destroy(&sem_proceso_entro_a_ready);
    sem_destroy(&sem_interrupt_algorithms);
    sem_destroy(&sem_interrupt_routine);
    sem_destroy(&sem_memory_handlers);
    sem_destroy(&sem_memory_routine);
    sem_destroy(&sem_memory_operation_resolved);

    pthread_mutex_destroy(&mutex_logger);
    pthread_mutex_destroy(&mutex_cola_new);

    list_destroy_and_destroy_elements(pcb_list, pcb_element_destroyer);
    queue_destroy_and_destroy_elements(cola_estado_new, pcb_element_destroyer);

    log_info(logger, "Apagando Kernel");
    log_destroy(logger);
    log_destroy(logger_largo_plazo);
    log_destroy(logger_blockeos);
    config_destroy(config);
}

bool pcb_add_comparator(void *arg1, void *arg2)
{
    t_pcb *elem1 = (t_pcb *)arg1, *elem2 = (t_pcb *)arg2;
    return elem1->id < elem2->id;
}

void pcb_element_destroyer(void *arg)
{
    t_pcb *elem = (t_pcb *)arg;
    sem_close(&(elem->console_semaphore));
    sem_close(&(elem->console_waiter_semaphore));
    sem_destroy(&(elem->console_semaphore));
    sem_destroy(&(elem->console_waiter_semaphore));
    for (size_t i = 0; i < elem->cant_instrucciones; i++)
    {
        free((elem->instrucciones)[i]);
    }

    free(elem->segmentos);
    free(elem->instrucciones);
    free(elem);
}

void logger_monitor_info(t_log *logger, const char *message)
{
    pthread_mutex_lock(&mutex_logger);
    log_info(logger, message);
    pthread_mutex_unlock(&mutex_logger);
}

void logger_monitor_error(t_log *logger, const char *message)
{
    pthread_mutex_lock(&mutex_logger);
    log_error(logger, message);
    pthread_mutex_unlock(&mutex_logger);
}

bool configurar_algoritmo(char* algortimo)
{
    if(strcmp(algortimo, "FIFO") == 0)
    {
        fifo_init_algoritmo();
        finalizar_algoritmo = fifo_final_algoritmo;
        ingresar_a_ready = fifo_ingresar_a_ready;
        obtener_siguiente_a_exec = fifo_obtener_siguiente_exec;
        sale_de_exec = fifo_sale_de_exec;
        return 0;
    } 
    else if (strcmp(algortimo, "RR") == 0)
    {
        rr_init_algoritmo();
        finalizar_algoritmo = rr_final_algoritmo;
        ingresar_a_ready = rr_ingresar_a_ready;
        obtener_siguiente_a_exec = rr_obtener_siguiente_exec;
        sale_de_exec = rr_sale_de_exec;
        return 0;
    }
        else if (strcmp(algortimo, "FEEDBACK") == 0)
    {
        feedback_init_algoritmo();
        finalizar_algoritmo = feedback_final_algoritmo;
        ingresar_a_ready = feedback_ingresar_a_ready;
        obtener_siguiente_a_exec = feedback_obtener_siguiente_exec;
        sale_de_exec = feedback_sale_de_exec;
        return 0;
    }
    return 1;
}

uint32_t search_for_id_buffer;
bool search_for_id(void *param)
{
    return ((t_pcb *)param)->id == search_for_id_buffer;
}

char* queue_listar_elementos(t_queue* cola)
{
    char* msg = string_from_format("[ ");
    t_list_iterator* iterador = list_iterator_create(cola->elements);
    for(
        ;
        list_iterator_has_next(iterador);
        string_append_with_format(&msg, "%lu ", (unsigned long)((t_pcb*) list_iterator_next(iterador)) -> id)
    );
    list_iterator_destroy(iterador);
    string_append(&msg, "]");
    return msg;
}