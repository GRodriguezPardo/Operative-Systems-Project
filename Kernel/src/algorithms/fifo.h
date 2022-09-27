#ifndef FIFO_H_
#define FIFO_H_

#include <commons/collections/queue.h>
#include "../globals.h"

////////////// VARIABLES //////////////
extern t_queue* cola_algoritmo_fifo;

////////////// FUNCIONES //////////////
void fifo_init_algoritmo();
void fifo_final_algoritmo();
void fifo_ingresar_a_ready(t_pcb* pcb);

#endif /* FIFO_H_ */