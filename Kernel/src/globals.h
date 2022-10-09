#ifndef GLOBALS_H_
#define GLOBALS_H_


#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <stdint.h>
#include <thesenate/tcp_serializacion.h>

////////////// LOGGERS GLOBALES //////////////
extern t_log* logger_largo_plazo;

////////////// ESTADO DEL KERNEL //////////////
extern void **pipeline;
extern uint8_t* status;
extern uint8_t *console_status;
extern int *console_acumulator;

////////////// ESTRUCTURAS DE DATOS //////////////
extern t_list* pcb_list;
extern t_queue* cola_estado_new;

////////////// SEMAFOROS PLANIFICADOR LARGO PLAZO //////////////
extern sem_t sem_grado_multiprogramacion;
extern sem_t sem_proceso_entro_a_new;

////////////// SEMAFOROS PLANIFICADOR CORTO PLAZO //////////////
extern sem_t sem_proceso_entro_a_ready;

////////////// MUTEX LOGGER //////////////
extern pthread_mutex_t mutex_logger;

////////////// MUTEX ESTADO DEL KERNEL //////////////
extern pthread_mutex_t mutex_pipeline;
extern pthread_mutex_t mutex_console_semaphores;
extern pthread_mutex_t mutex_status;
extern pthread_mutex_t mutex_console_status;
extern pthread_mutex_t mutex_console_acumulator;
extern pthread_mutex_t mutex_planificador;

////////////// MUTEX ESTRUCTURAS DE DATOS //////////////
extern pthread_mutex_t mutex_pcb_list;
extern pthread_mutex_t mutex_cola_new;

////////////// DEFINICION DEL PCB //////////////
typedef struct t_segmento_pcb {
    uint32_t tamanio;
    uint32_t identificador_tabla;
} t_segmento_pcb;

typedef struct t_pipeline {
    op_code operacion;
    uint32_t valor;
} t_pipeline;

typedef struct t_pcb
{
    uint32_t id;
    char** instrucciones;
    uint32_t cant_instrucciones;
    uint32_t program_counter;
    uint32_t registros[4];
    t_segmento_pcb segmentos[4];
    sem_t console_semaphore;
    t_pipeline pipeline;
} t_pcb;

////////////// PUNTEROS A FUNCIONES GLOBALES - ALGORITMOS //////////////
extern void (*finalizar_algoritmo)();
extern void (*ingresar_a_ready)(t_pcb* pcb);
extern void (*planificador_corto_plazo)(char* configPath);
extern t_pcb* (*obtener_siguiente_a_exec)();

#endif /* GLOBALS_H_ */