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
t_log *logger;
t_memoria_config ConfigMemoria;
pthread_mutex_t mx_logger, mx_main, mx_espacioUsuario, 
    mx_espacioTablasPag, mx_listaPageFaults;
void *EspacioUsuario;
t_list *EspacioTablasPag, *ListaPageFaults;
t_bitarray *MapaFrames, *MapaSwap;

// -------

void cargar_configuracion_memoria();
void inicializar_espacio_usuario();
void inicializar_espacio_tablas();
void inicializar_lista_pageFaults();
void inicializar_mapas();

void *levantar_server_memoria(void *);
void *atender_conexion(void *);

void finalizar_memoria();
void destruir_espacio_tablas();
void destruir_lista_pageFaults();
void destruir_mapas();
void cerrarMutexes();
void pag_liberar_espacioKernel();

#endif /* MEMORIA_H_ */