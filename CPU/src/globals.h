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
    uint32_t direcLogica;
    uint32_t direcFisica;
    uint32_t idTablaPagina;
    uint32_t idPagina;
} t_pipeline;

typedef struct t_segmento {
    uint32_t nro_segmento;
    uint32_t tamanio;
    uint32_t identificador_tabla;
} t_segmento;

typedef struct t_tlb {
    uint32_t pid;
    uint32_t nro_segmento;
    uint32_t nro_pag;
    uint32_t marco;
} t_tlb;

typedef struct _configMem{
    uint32_t entradasTablaPaginas;
    uint32_t tamanioPagina;
    uint32_t tamanioMaximoSegmento;
    int entradasTLB;
    t_pipeline pipelineMemoria;
    uint32_t numMarco;
} t_configMemoria;


typedef struct t_contexto
{
    uint32_t id;
    char **instrucciones;
    uint32_t program_counter;
    uint32_t registros[4];
    uint32_t cantSegmentos;
    t_segmento *segmentos; 
    char *dispositivo; 
    uint32_t unidades;
    t_pipeline pipeline;
} t_contexto;

extern int flag_interrupcion;


extern t_contexto *mi_contexto;

extern t_configMemoria *configMemoria;

extern t_tlb *tlb;

extern void **pipeline;
extern void **pipelineMemoria;

extern pthread_mutex_t mutex_logger;

extern pthread_mutex_t mutex_dispatch_response;

extern pthread_mutex_t mutex_ejecucion;

extern pthread_mutex_t mutex_flag;

extern uint32_t pid_interrupt;

extern sem_t sem;
extern sem_t sem_ciclo_instruccion;
extern sem_t sem_envio_contexto;
extern sem_t sem_conexion_memoria;
extern sem_t sem_mmu;


#endif 