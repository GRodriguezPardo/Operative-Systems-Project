#ifndef KERNEL_H_
#define KERNEL_H_

void *interrupt_server(void *config);

void *dispatch_server(void *config);
void *dispatch_routine(void *socket);
void *interrupt_routine(void *socket);
void *ciclo_instruccion(void *config);

t_contexto *mi_contexto;

pthread_mutex_t mutex_pipeline;
pthread_mutex_t mutex_dispatch_response;
pthread_mutex_t mutex_ejecucion;

#endif /* KERNEL_H_ */