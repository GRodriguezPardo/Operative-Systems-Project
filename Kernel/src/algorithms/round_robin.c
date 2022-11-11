#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <thesenate/tcp_serializacion.h>
#include <unistd.h>
#include "round_robin.h"
#include "../globals.h"
#include "../kernel_utils.h"

t_queue *cola_algoritmo_rr;
pthread_mutex_t mutex_cola_rr;
t_log *logger_rr;
pthread_t last_clock_id_rr;

void *rr_clock_interrupt(void *param);

void rr_init_algoritmo()
{
    cola_algoritmo_rr = queue_create();
    pthread_mutex_init(&mutex_cola_rr, NULL);
    logger_rr = log_create("../kernel.log", "Kernel - Algoritmo RR", 0, LOG_LEVEL_INFO);
}

void rr_final_algoritmo()
{
    pthread_mutex_lock(&mutex_cola_rr);
    queue_destroy(cola_algoritmo_rr);
    pthread_mutex_unlock(&mutex_cola_rr);
    pthread_mutex_destroy(&mutex_cola_rr);
    log_destroy(logger_rr);
}

void rr_ingresar_a_ready(t_pcb *pcb, op_code __attribute__((unused)) source)
{
    pthread_mutex_lock(&mutex_cola_rr);
    queue_push(cola_algoritmo_rr, (void *)pcb);
    char *lista = queue_listar_elementos(cola_algoritmo_rr);
    logger_monitor_info(logger_rr, lista);
    free(lista);
    pthread_mutex_unlock(&mutex_cola_rr);
}

t_pcb *rr_obtener_siguiente_exec()
{
    pthread_mutex_lock(&mutex_cola_rr);
    t_pcb *pcb = (t_pcb *)queue_pop(cola_algoritmo_rr);
    pthread_mutex_unlock(&mutex_cola_rr);

    uint32_t *id;
    {
        id = (uint32_t *)malloc(sizeof(uint32_t));
        *id = pcb->id;
    }
    pthread_create(&last_clock_id_rr, NULL, rr_clock_interrupt, (void *)id);

    return pcb;
}

void rr_sale_de_exec(t_pcb *pcb __attribute__((unused)), op_code __attribute__((unused)) source)
{
    sem_wait(&sem_interrupt_algorithms);
    pthread_cancel(last_clock_id_rr);
    sem_post(&sem_interrupt_algorithms);
    return;
}

void *rr_clock_interrupt(void *param)
{
    uint32_t pid = *((uint32_t *)param);

    free(param);

    usleep(QUANTUM * 1000);

    sem_wait(&sem_interrupt_algorithms);
    global_pid_to_interrupt = pid;
    sem_post(&sem_interrupt_routine);

    return NULL;
}