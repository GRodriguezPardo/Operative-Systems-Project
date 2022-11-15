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
extern uint8_t *console_status;
extern int *console_acumulator;
extern uint32_t QUANTUM;

////////////// ESTADO DE INTERRUPT ROUTINE //////////////
extern uint32_t global_pid_to_interrupt;
extern t_log* logger_blockeos;

////////////// ESTADO DE IO //////////////
extern char** dispositivos_IO;
extern uint32_t* tiempos_IO;
extern size_t cantidad_dispositivos_IO;

////////////// ESTRUCTURAS DE DATOS //////////////
extern t_list* pcb_list;
extern t_queue* cola_estado_new;

////////////// SEMAFOROS PLANIFICADOR LARGO PLAZO //////////////
extern sem_t sem_grado_multiprogramacion;
extern sem_t sem_proceso_entro_a_new;

////////////// SEMAFOROS PLANIFICADOR CORTO PLAZO //////////////
extern sem_t sem_proceso_entro_a_ready;

////////////// SEMAFOROS INTERRUPT ROUTINE //////////////
extern sem_t sem_interrupt_routine;
extern sem_t sem_interrupt_algorithms;

////////////// SEMAFOROS I/O //////////////
extern sem_t* sem_dispositivos_IO;

////////////// MUTEX LOGGER //////////////
extern pthread_mutex_t mutex_logger;

////////////// MUTEX ESTADO DEL KERNEL //////////////

////////////// MUTEX ESTRUCTURAS DE DATOS //////////////
extern pthread_mutex_t mutex_pcb_list;
extern pthread_mutex_t mutex_cola_new;

////////////// DEFINICION DEL PCB //////////////
typedef struct t_segmento_pcb {
    uint32_t nro_segmento;
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
    uint32_t cant_segmentos;
    t_segmento_pcb* segmentos;
    sem_t console_semaphore;
    sem_t console_waiter_semaphore;
    t_pipeline pipeline;
} t_pcb;

////////////// PUNTEROS A FUNCIONES GLOBALES - ALGORITMOS //////////////
extern void (*finalizar_algoritmo)();
extern void (*ingresar_a_ready)(t_pcb* pcb, op_code source);
extern t_pcb* (*obtener_siguiente_a_exec)();
extern void (*sale_de_exec)(t_pcb* pcb, op_code source);

#endif /* GLOBALS_H_ */