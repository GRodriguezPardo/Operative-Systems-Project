#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <thesenate/tcp_serializacion.h>
#include <unistd.h>
#include "round_robin.h"
#include "../globals.h"

t_queue *cola_algoritmo_rr;
pthread_mutex_t mutex_cola_rr;
pthread_mutex_t mutex_reloj_rr;
sem_t sem_clock_rr;

short clock_time;
short QUANTUM;

void rr_init_algoritmo()
{
    cola_algoritmo_rr = queue_create();
    pthread_mutex_init(&mutex_cola_rr, NULL);
    pthread_mutex_init(&mutex_reloj_rr, NULL);
    sem_init(&sem_clock_rr, 0, 0);
    QUANTUM = 2;
    clock_time = QUANTUM;
}

void rr_final_algoritmo()
{
    pthread_mutex_lock(&mutex_cola_rr);
    queue_destroy(cola_algoritmo_rr);
    pthread_mutex_unlock(&mutex_cola_rr);
}

void rr_ingresar_a_ready(t_pcb *pcb, op_code __attribute__((unused)) source)
{
    pthread_mutex_lock(&mutex_cola_rr);
    queue_push(cola_algoritmo_rr, (void *)pcb);
    pthread_mutex_unlock(&mutex_cola_rr);
}

t_pcb *rr_obtener_siguiente_exec()
{
    pthread_mutex_lock(&mutex_cola_rr);
    t_pcb* pcb = (t_pcb *)queue_pop(cola_algoritmo_rr);
    pthread_mutex_unlock(&mutex_cola_rr);

    for (size_t i = 0; i < QUANTUM; i++)
    {
        sem_trywait(&sem_clock_rr);
    }

    pthread_mutex_lock(&mutex_reloj_rr);
    clock_time = QUANTUM;
    pthread_mutex_unlock(&mutex_reloj_rr);

    for (size_t i = 0; i < QUANTUM; i++)
    {
        sem_post(&sem_clock_rr);
    }
    return pcb;
}

void rr_sale_de_exec(t_pcb* pcb __attribute__((unused)), op_code __attribute__((unused)) source)
{
    for (size_t i = 0; i < QUANTUM; i++)
    {
        sem_trywait(&sem_clock_rr);
    }

    pthread_mutex_lock(&mutex_reloj_rr);
    clock_time = QUANTUM;
    pthread_mutex_unlock(&mutex_reloj_rr);
}

void rr_clock_interrupt()
{
    while(clock_time > 0)
    {
        sem_wait(&sem_clock_rr);
        sleep(1000);
        pthread_mutex_lock(&mutex_reloj_rr);
        clock_time--;
        pthread_mutex_unlock(&mutex_reloj_rr);
    }
    pthread_mutex_lock(&mutex_reloj_rr);
    clock_time = QUANTUM;
    pthread_mutex_unlock(&mutex_reloj_rr);
}