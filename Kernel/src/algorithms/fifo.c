#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <thesenate/tcp_serializacion.h>
#include "fifo.h"
#include "../globals.h"
#include "../kernel_utils.h"

t_queue *cola_algoritmo_fifo;
pthread_mutex_t mutex_cola_fifo;
t_log *logger_fifo;

void fifo_init_algoritmo()
{
    cola_algoritmo_fifo = queue_create();
    pthread_mutex_init(&mutex_cola_fifo, NULL);
    logger_fifo = log_create("../kernel.log", "Kernel - Algoritmo FIFO", 0, LOG_LEVEL_INFO);
}

void fifo_final_algoritmo()
{
    pthread_mutex_lock(&mutex_cola_fifo);
    queue_destroy(cola_algoritmo_fifo);
    pthread_mutex_unlock(&mutex_cola_fifo);

    pthread_mutex_destroy(&mutex_cola_fifo);
    log_destroy(logger_fifo);
}

void fifo_ingresar_a_ready(t_pcb *pcb, op_code __attribute__((unused)) source)
{
    pthread_mutex_lock(&mutex_cola_fifo);
    queue_push(cola_algoritmo_fifo, (void *)pcb);
    char* lista = queue_listar_elementos(cola_algoritmo_fifo);
    logger_monitor_info(logger_fifo, lista);
    free(lista);
    pthread_mutex_unlock(&mutex_cola_fifo);
}

t_pcb *fifo_obtener_siguiente_exec()
{
    pthread_mutex_lock(&mutex_cola_fifo);
    t_pcb* pcb = (t_pcb*) queue_pop(cola_algoritmo_fifo);
    pthread_mutex_unlock(&mutex_cola_fifo);
    return pcb;
}

void fifo_sale_de_exec(t_pcb* pcb __attribute__((unused)), op_code __attribute__((unused)) source)
{
    return;
}
