#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_globals.h"
#include "memoria_utils.h"
#include "paginacion.h"
#include "routines/cpu_routine.h"
#include "routines/kernel_routine.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

//////// VARIABLES GLOBALES /////////
t_config *config;
t_log *logger;
t_memoria_config configMemoria;
pthread_mutex_t mx_logger, mx_main, mx_memoriaPrincipal, mx_espacioKernel;
void *memoriaPrincipal;
t_list *espacioKernel;
t_list *tablaFrames;

void (*reemplazarPagina)();

// -------

void cargar_configuracion_memoria();

void *levantar_server_memoria(void *);
void *atender_conexion(void *);

void finalizar_memoria();
void pag_liberar_espacioKernel();

#endif /* MEMORIA_H_ */