#ifndef MEMORIA_GLOBALS_H_
#define MEMORIA_GLOBALS_H_

#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/log.h>
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
        entradasPorTabla, 
        retardoMemoria,
        marcosPorProceso,
        retardoSwap,
        tamanioSwap;
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
extern pthread_mutex_t mx_espacioUsuario;
extern pthread_mutex_t mx_espacioTablasPag;
extern void *espacioUsuario;
extern t_list *espacioTablasPag;
extern t_list *tablaFrames;
extern int swapFd;

//////// PUNTEROS A FUNCIONES /////////
extern void (*reemplazarPagina)();

#endif /* MEMORIA_GLOBALS_H_ */