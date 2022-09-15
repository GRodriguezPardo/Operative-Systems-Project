#ifndef KERNEL_H_
#define KERNEL_H_

void *interrupt_server(void *config);

void *dispatch_server(void *config);
void *dispatch_routine(void *socket);
void *interrupt_routine(void *socket);

#endif /* KERNEL_H_ */