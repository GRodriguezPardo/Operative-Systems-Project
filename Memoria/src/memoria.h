#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_globals.h"
#include "memoria_utils.h"
#include "paginacion.h"
#include "routines/cpu_routine.h"
#include "routines/kernel_routine.h"
#include <semaphore.h>
#include <stdio.h>

//////// ESTRUCTURAS GLOBALES /////////
t_config *config;
t_log *loggerMain, *loggerAux;
t_memoria_config ConfigMemoria;
pthread_mutex_t mx_main, mx_loggerAux, mx_loggerMain, mx_espacioUsuario, 
    mx_espacioKernel, mx_tablaFrames;
void *EspacioUsuario;
t_dictionary *EspacioKernel;
t_bitarray *MapaFrames, *MapaSwap;
t_dictionary *TablaFrames;

// -------

void cargar_configuracion_memoria();
void inicializar_espacio_usuario();
void inicializar_espacio_tablas();
void inicializar_tabla_frames();
void inicializar_mapas();

void *levantar_server_memoria(void *);
void *atender_conexion(void *);

void finalizar_memoria();
void destruir_espacio_tablas();
void destruir_tabla_frames();
void destruir_mapas();
void cerrarMutexes();
void pag_liberar_espacioKernel();

#endif /* MEMORIA_H_ */