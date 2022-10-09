#ifndef CPU_DISPATCH_H_
#define CPU_DISPATCH_H_

#include "../globals.h"

void *cpu_dispatch_routine(void *config);
void give_cpu_next_pcb(int socket);
t_pcb* obtener_y_actualizar_pcb_recibido(int socket);
void finalizar_proceso(t_pcb* unPcb);

#endif /* CPU_DISPATCH_H_ */