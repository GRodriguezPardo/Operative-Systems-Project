#ifndef KERNEL_H_
#define KERNEL_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>
#include "globals.h"

void *consola_routine(void *socket);

t_log* logger_largo_plazo;

void **pipeline;
uint8_t* status;
uint8_t *console_status;
int *console_acumulator;

t_list* console_semaphores;
t_list* pcb_list;
t_queue* cola_estado_new;

sem_t sem_grado_multiprogramacion;
sem_t sem_proceso_entro_a_new;

sem_t sem_proceso_entro_a_ready;

pthread_mutex_t mutex_logger;

pthread_mutex_t mutex_pipeline;
pthread_mutex_t mutex_console_semaphores;
pthread_mutex_t mutex_status;
pthread_mutex_t mutex_console_status;
pthread_mutex_t mutex_console_acumulator;
pthread_mutex_t mutex_planificador;

pthread_mutex_t mutex_pcb_list;
pthread_mutex_t mutex_cola_new;

void (*finalizar_algoritmo)();
void (*ingresar_a_ready)(t_pcb* pcb);
void (*planificador_corto_plazo)(char* configPath);
t_pcb* (*obtener_siguiente_a_exec)();

#endif /* KERNEL_H_ */
