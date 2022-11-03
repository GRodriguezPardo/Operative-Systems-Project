#ifndef MEMORIA_GLOBALS_H_
#define MEMORIA_GLOBALS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_server.h>
#include <thesenate/tcp_serializacion.h>
#include <pthread.h>

typedef struct mem_config
{
    char *ip, 
        *puerto, 
        *algoritmo_reemplazo, 
        *path_swap;
    uint32_t tamanio_memoria, 
        tamanio_pagina, 
        entradas_x_tabla, 
        retardo_memoria,
        marcos_x_proceso,
        retardo_swap,
        tamanio_swap;
} t_memoria_config;

typedef struct t_segmento_pcb {
    uint32_t tamanio;
    uint32_t identificador_tabla;
} t_segmento_pcb;

//////// ESTRUCTURAS GLOBALES /////////
extern t_config *config;
extern t_log *logger;
extern t_memoria_config configMemoria;
extern pthread_mutex_t mx_logger;
extern pthread_mutex_t mx_main;
extern pthread_mutex_t mx_memoria;
extern void *memoriaPrincipal;
extern int swapFd;

//////// VALORES GLOBALES /////////

//////// PUNTEROS A FUNCIONES /////////
extern void (*reemplazarPagina)();

#endif /* MEMORIA_GLOBALS_H_ */