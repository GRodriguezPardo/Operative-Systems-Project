#ifndef MEMORIA_GLOBALS_H_
#define MEMORIA_GLOBALS_H_

#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
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

typedef struct tablaPaginas
{
    uint32_t idProceso;
    t_list *tabla;
} t_tablaPaginas;

typedef struct pagina
{
    uint32_t marco,
        presente, 
        usado, 
        modificado,
        posicion_swap;
} t_pagina;

typedef struct pagina_page_fault {
    uint32_t idProceso;
    uint32_t idTabla;
    uint32_t numPagina;
} t_pagina_page_fault;

//////// ESTRUCTURAS GLOBALES /////////
extern t_config *config;
extern t_log *logger;
extern t_memoria_config configMemoria;
extern pthread_mutex_t mx_logger;
extern pthread_mutex_t mx_main;
extern pthread_mutex_t mx_espacioUsuario;
extern pthread_mutex_t mx_espacioTablasPag;
extern pthread_mutex_t mx_listaPageFaults;
extern void *EspacioUsuario;
extern t_list *EspacioTablasPag;
extern t_bitarray *MapaFrames;
extern t_bitarray *MapaSwap;
extern t_list *ListaPageFaults;
extern int swapFile;

#endif /* MEMORIA_GLOBALS_H_ */