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

void* rr_clock_interrupt(void* param);

void rr_init_algoritmo()
{
    cola_algoritmo_rr = queue_create();
    pthread_mutex_init(&mutex_cola_rr, NULL);
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

    uint32_t* id;
    pthread_t thread_id;
    {
        id = (uint32_t*)malloc(sizeof(uint32_t));
        *id = pcb->id;
    }
    pthread_create(&thread_id, NULL, rr_clock_interrupt, (void*) id);

    return pcb;
}

void rr_sale_de_exec(t_pcb* pcb __attribute__((unused)), op_code __attribute__((unused)) source)
{
    return;
}

void* rr_clock_interrupt(void* param)
{
    uint32_t pid = *((uint32_t*) param);
    
    usleep(QUANTUM * 1000); 
    
    sem_wait(&sem_interrupt_algorithms);
    global_pid_to_interrupt = pid;
    sem_post(&sem_interrupt_routine);

    free(param);
    return NULL;
}