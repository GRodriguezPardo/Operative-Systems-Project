#ifndef KERNEL_H_
#define KERNEL_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include <thesenate/tcp_serializacion.h>
#include "globals.h"

void *consola_routine(void *socket);

t_log* logger_largo_plazo;

uint8_t *console_status;
int *console_acumulator;
uint32_t QUANTUM;

uint32_t global_pid_to_interrupt;
t_log* logger_blockeos;

char** dispositivos_IO;
uint32_t* tiempos_IO;
size_t cantidad_dispositivos_IO;

t_list* console_semaphores;
t_list* pcb_list;
t_queue* cola_estado_new;

sem_t sem_grado_multiprogramacion;
sem_t sem_proceso_entro_a_new;

sem_t sem_proceso_entro_a_ready;

sem_t sem_interrupt_routine;
sem_t sem_interrupt_algorithms;

sem_t* sem_dispositivos_IO;

pthread_mutex_t mutex_logger;

pthread_mutex_t mutex_pcb_list;
pthread_mutex_t mutex_cola_new;

void (*finalizar_algoritmo)();
void (*ingresar_a_ready)(t_pcb* pcb, op_code source);
t_pcb* (*obtener_siguiente_a_exec)();
void (*sale_de_exec)(t_pcb* pcb, op_code source);

#endif /* KERNEL_H_ */
