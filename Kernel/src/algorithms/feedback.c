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
#include "feedback.h"
#include "../globals.h"
#include "../kernel_utils.h"

void *feedback_clock_interrupt(void *param);

t_queue *cola_algoritmo_feedback_rr;
t_queue *cola_algoritmo_feedback_fifo;
pthread_mutex_t mutex_cola_feedback;
t_log *logger_fb;
pthread_t last_clock_id_fb;
bool is_rr;

void feedback_init_algoritmo()
{
    cola_algoritmo_feedback_rr = queue_create();
    cola_algoritmo_feedback_fifo = queue_create();
    pthread_mutex_init(&mutex_cola_feedback, NULL);
    logger_fb = log_create("../kernel.log", "Kernel - Algoritmo FEEDBACK", 0, LOG_LEVEL_INFO);
    is_rr = 0;
}

void feedback_final_algoritmo()
{
    pthread_mutex_lock(&mutex_cola_feedback);
    queue_destroy(cola_algoritmo_feedback_rr);
    queue_destroy(cola_algoritmo_feedback_fifo);
    pthread_mutex_unlock(&mutex_cola_feedback);

    pthread_mutex_destroy(&mutex_cola_feedback);
    log_destroy(logger_fb);
}

void feedback_ingresar_a_ready(t_pcb *pcb, op_code source)
{
    pthread_mutex_lock(&mutex_cola_feedback);
    switch (source)
    {
    case NUEVO_PROCESO:
    case BLOQUEO_PROCESO:
        queue_push(cola_algoritmo_feedback_rr, (void *)pcb);
        break;

    case DESALOJO_PROCESO:
        queue_push(cola_algoritmo_feedback_fifo, (void *)pcb);
        break;
    default:
        exit(EXIT_FAILURE);
    }

    char *cola1 = queue_listar_elementos(cola_algoritmo_feedback_rr);
    char *cola2 = queue_listar_elementos(cola_algoritmo_feedback_fifo);
    char *msg = string_from_format("COLA RR : %s | COLA FIFO : %s", cola1, cola2);
    logger_monitor_info(logger_fb, msg);
    free(cola1);
    free(cola2);
    free(msg);
    pthread_mutex_unlock(&mutex_cola_feedback);
}

t_pcb *feedback_obtener_siguiente_exec()
{
    pthread_mutex_lock(&mutex_cola_feedback);
    t_pcb *pcb;
    if (!queue_is_empty(cola_algoritmo_feedback_rr))
    {
        pcb = (t_pcb *)queue_pop(cola_algoritmo_feedback_rr);

        uint32_t *id;
        {
            id = (uint32_t *)malloc(sizeof(uint32_t));
            *id = pcb->id;
        }
        pthread_create(&last_clock_id_fb, NULL, feedback_clock_interrupt, (void *)id);
        is_rr = 1;
    }
    else
    {
        pcb = (t_pcb *)queue_pop(cola_algoritmo_feedback_fifo);
        is_rr = 0;
    }

    pthread_mutex_unlock(&mutex_cola_feedback);
    return pcb;
}

void feedback_sale_de_exec(t_pcb *pcb __attribute__((unused)), op_code __attribute__((unused)) source)
{
    if(is_rr)
    {
        sem_wait(&sem_interrupt_algorithms);
        pthread_cancel(last_clock_id_fb);
        sem_post(&sem_interrupt_algorithms);
    }
    return;
}

void *feedback_clock_interrupt(void *param)
{
    uint32_t pid = *((uint32_t *)param);

    usleep(QUANTUM * 1000);

    sem_wait(&sem_interrupt_algorithms);
    global_pid_to_interrupt = pid;
    sem_post(&sem_interrupt_routine);

    free(param);
    return NULL;
}