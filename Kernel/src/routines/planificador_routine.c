#include <commons/string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "planificador_routine.h"
#include "../globals.h"
#include "../kernel_utils.h"

void ingresar_a_new(t_pcb *nuevo_pcb)
{
    pthread_mutex_lock(&mutex_cola_new);
    queue_push(cola_estado_new, (void *)nuevo_pcb);
    pthread_mutex_unlock(&mutex_cola_new);

    char *msg = string_from_format("Proceso %lu : -> NEW (Creacion)", (unsigned long)(nuevo_pcb->id));
    logger_monitor_info(logger_largo_plazo, msg);
    free(msg);

    sem_post(&sem_proceso_entro_a_new);
}

t_pcb *obtener_siguiente_en_new()
{
    pthread_mutex_lock(&mutex_cola_new);
    t_pcb *proximo_pcb = (t_pcb *)queue_pop(cola_estado_new);
    pthread_mutex_unlock(&mutex_cola_new);
    return proximo_pcb;
}

void *new_a_ready(void *arg)
{
    while (1)
    {
        sem_wait(&sem_proceso_entro_a_new);

        sem_wait(&sem_grado_multiprogramacion);

        t_pcb *pcb = obtener_siguiente_en_new();
        ingresar_a_ready(pcb, NUEVO_PROCESO);

        pthread_mutex_lock(&mutex_pcb_list);
        list_add(pcb_list, pcb);
        pthread_mutex_unlock(&mutex_pcb_list);

        int val = 0;
        sem_getvalue(&sem_grado_multiprogramacion, &val);
        char *msg = string_from_format("Proceso %lu : NEW -> READY (Multiprogramacion %d)", (unsigned long)(pcb->id), val);
        logger_monitor_info(logger_largo_plazo, msg);
        free(msg);

        sem_post(&sem_proceso_entro_a_ready);
    }
}