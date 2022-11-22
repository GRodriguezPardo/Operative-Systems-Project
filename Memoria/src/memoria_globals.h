#ifndef MEMORIA_GLOBALS_H_
#define MEMORIA_GLOBALS_H_

#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <commons/log.h>
#include <stdlib.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_server.h>
#include <thesenate/tcp_serializacion.h>
#include <pthread.h>

typedef struct mem_config
{
    char *ip, 
        *puerto, 
        *algoritmoReemplazo, 
        *pathSwap;
    uint32_t tamanioMemoria, 
        tamanioPagina, 
        paginasPorTabla,
        entradasPorTabla, 
        retardoMemoria,
        marcosPorProceso,
        retardoSwap,
        tamanioSwap,
        cantidadMarcosMemoria,
        cantidadPaginasSwap;
} t_memoria_config;

typedef struct t_segmento_pcb {
    uint32_t tamanio;
    uint32_t identificador_tabla;
    uint32_t nro_segmento;
} t_segmento_pcb;

typedef struct dataProceso
{
    t_list *tablasProceso;
    t_queue *paginasPresentes;
} t_infoProceso;

typedef struct pagina
{
    uint32_t id,
        numeroSegmento,
        marco,
        posicion_swap;
    bool presente, 
        usado, 
        modificado;
} t_pagina;

typedef struct infoFrame
{
    uint32_t pid,
        idTabla,
        numPagina;
} t_infoFrame;

//////// ESTRUCTURAS GLOBALES /////////
extern t_config *config;
extern t_log *loggerMain;
extern t_log *loggerAux;
extern t_memoria_config ConfigMemoria;
extern pthread_mutex_t mx_loggerMain;
extern pthread_mutex_t mx_loggerAux;
extern pthread_mutex_t mx_main;
extern pthread_mutex_t mx_espacioUsuario;
extern pthread_mutex_t mx_espacioTablasPag;
extern pthread_mutex_t mx_tablaFrames;
extern void *EspacioUsuario;
extern t_dictionary *EspacioTablas;
extern t_bitarray *MapaFrames;
extern t_bitarray *MapaSwap;
extern t_dictionary *TablaFrames;
extern int swapFile;

#endif /* MEMORIA_GLOBALS_H_ */