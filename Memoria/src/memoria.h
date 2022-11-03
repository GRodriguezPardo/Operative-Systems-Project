#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_globals.h"
#include "memoria_utils.h"
#include "routines/cpu_routine.h"
#include "routines/kernel_routine.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

//////// VARIABLES GLOBALES /////////
t_config *config;
t_log *logger;
t_memoria_config configMemoria;
pthread_mutex_t mx_logger, mx_main, mx_memoria;
void *memoriaPrincipal;

void (*reemplazarPagina)();

// -------

void crear_config();
void crear_logger();
void cargar_configuracion_memoria();

void *levantar_server_memoria(void *);
void *atender_conexion(void *);

#endif /* MEMORIA_H_ */