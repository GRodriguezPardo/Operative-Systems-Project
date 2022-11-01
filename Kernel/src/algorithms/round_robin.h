#ifndef ROUND_ROBIN_H_
#define ROUND_ROBIN_H_

#include <commons/collections/queue.h>
#include <pthread.h>
#include <thesenate/tcp_serializacion.h>
#include "../globals.h"

////////////// FUNCIONES //////////////
void rr_init_algoritmo();
void rr_final_algoritmo();
void rr_ingresar_a_ready(t_pcb* pcb, op_code source);
t_pcb *rr_obtener_siguiente_exec();
void rr_sale_de_exec(t_pcb* pcb, op_code source);

#endif /* ROUND_ROBIN_H_ */