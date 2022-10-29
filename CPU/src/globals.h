#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <stdint.h>
#include <thesenate/tcp_serializacion.h>

////////////// DEFINICION DEL CONTEXTO //////////////

typedef struct t_pipeline {
    op_code operacion;
    uint32_t valor;
} t_pipeline;

typedef struct t_contexto
{
    uint32_t id;
    char **instrucciones;
    uint32_t program_counter;
    uint32_t registros[4];
    uint32_t segmentos[4][2];
    char *dispositivo;
    uint32_t unidades;
    t_pipeline pipeline;
} t_contexto;

extern int flag_interrupcion;

extern t_contexto *mi_contexto;

extern void **pipeline;

extern pthread_mutex_t mutex_logger;

extern pthread_mutex_t mutex_dispatch_response;

extern pthread_mutex_t mutex_ejecucion;




extern sem_t sem;
extern sem_t sem_ciclo_instruccion;
extern sem_t sem_envio_contexto;

#endif 