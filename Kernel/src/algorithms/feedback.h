#ifndef FEEDBACK_H_
#define FEEDBACK_H_

#include <commons/collections/queue.h>
#include <pthread.h>
#include <thesenate/tcp_serializacion.h>
#include "../globals.h"

////////////// FUNCIONES //////////////
void feedback_init_algoritmo();
void feedback_final_algoritmo();
void feedback_ingresar_a_ready(t_pcb* pcb, op_code source);
t_pcb *feedback_obtener_siguiente_exec();
void feedback_sale_de_exec(t_pcb* pcb, op_code source);

#endif /* FEEDBACK_H_ */