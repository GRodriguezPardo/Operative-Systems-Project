#ifndef PLANIFICADOR_ROUTINE_H_
#define PLANIFICADOR_ROUTINE_H_

#include "../globals.h"

void ingresar_a_new(t_pcb *nuevo_pcb);
t_pcb *obtener_siguiente_en_new();
void *new_a_ready(void* arg);

#endif /* PLANIFICADOR_ROUTINE_H_ */