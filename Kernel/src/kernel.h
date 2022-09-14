#ifndef KERNEL_H_
#define KERNEL_H_

void *consola_routine(void *socket);

void *memoria_routine(void *config);
void *cpu_dispatch_routine(void *config);
void *cpu_interrupt_routine(void *config);

#endif /* KERNEL_H_ */