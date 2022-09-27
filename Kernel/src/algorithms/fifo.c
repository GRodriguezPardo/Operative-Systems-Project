#include <commons/collections/queue.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"
#include "../globals.h"

t_queue* cola_algoritmo_fifo;

void fifo_init_algoritmo()
{
    cola_algoritmo_fifo = queue_create();
}

void fifo_final_algoritmo()
{
    queue_destroy(cola_algoritmo_fifo);
}

void fifo_ingresar_a_ready(t_pcb* pcb)
{
    queue_push(cola_algoritmo_fifo, (void*) pcb);
}