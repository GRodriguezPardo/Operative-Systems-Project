#ifndef PLANIFICADOR_ROUTINE_H_
#define PLANIFICADOR_ROUTINE_H_

#include "../globals.h"

void ingresar_a_new(t_pcb *nuevo_pcb);
t_pcb *obtener_siguiente_en_new();
void *new_a_ready(void* arg);
void finalizar_IO(uint32_t pid);

#endif /* PLANIFICADOR_ROUTINE_H_ */