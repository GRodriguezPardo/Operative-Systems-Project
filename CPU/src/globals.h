#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <stdint.h>
#include <thesenate/tcp_serializacion.h>

////////////// DEFINICION DEL CONTEXTO //////////////
extern pthread_mutex_t mutex_logger;

extern pthread_mutex_t mutex_dispatch_response;

extern pthread_mutex_t mutex_ejecucion;

typedef struct t_contexto
{
    uint32_t id;
    char *instrucciones;
    uint32_t program_counter;
    uint32_t registros[4];
    //faltan segmentos
} t_contexto;

#endif 