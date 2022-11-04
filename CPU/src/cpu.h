#ifndef KERNEL_H_
#define KERNEL_H_

t_contexto *mi_contexto;

int flag_interrupcion;

void **pipeline;

uint32_t pid_interrupt;

sem_t sem;
sem_t sem_ciclo_instruccion;
sem_t sem_envio_contexto;

pthread_mutex_t mutex_logger;
pthread_mutex_t mutex_dispatch_response;
pthread_mutex_t mutex_ejecucion;
pthread_mutex_t mutex_flag;


#endif /* KERNEL_H_ */