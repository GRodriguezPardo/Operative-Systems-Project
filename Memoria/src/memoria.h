#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_globals.h"
#include "memoria_utils.h"
#include "routines/cpu_routine.h"
#include "routines/kernel_routine.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

//////// VARIABLES GLOBALES /////////

t_config *config;
t_log *logger;
pthread_mutex_t mx_logger, mx_main, mx_memoria;
void *memoriaPrincipal;

int tamanioMaxSegmento;
int cantMaxMarcosXProceso;

void (*reemplazarPagina)();


void *levantarServerMemoria(void *);
void *atenderConexion(void *);

#endif /* MEMORIA_H_ */