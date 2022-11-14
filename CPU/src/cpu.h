#ifndef KERNEL_H_
#define KERNEL_H_

t_contexto *mi_contexto;
t_configMemoria *configMemoria;

int flag_interrupcion;


void **pipeline;
void **pipelineMemoria;

t_tlb *tlb;

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