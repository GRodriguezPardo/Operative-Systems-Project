#ifndef KERNEL_H_
#define KERNEL_H_

t_contexto *mi_contexto;
t_configMemoria *configMemoria;

int flag_interrupcion;
int flag_segFault;
int flag_pageFault;

void **pipeline;
void **pipelineMemoria;

char* reemplazo_tlb;
t_list* colaTLB;

uint32_t pid_interrupt;

sem_t sem;
sem_t sem_ciclo_instruccion;
sem_t sem_envio_contexto;
sem_t sem_conexion_memoria;
sem_t sem_mmu;

pthread_mutex_t mutex_logger;
pthread_mutex_t mutex_dispatch_response;
pthread_mutex_t mutex_ejecucion;
pthread_mutex_t mutex_flag;


#endif /* KERNEL_H_ */