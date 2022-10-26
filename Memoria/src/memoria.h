#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "memoria_globals.h"
#include "memoria_utils.h"
#include "routines/cpu_routine.h"
#include "routines/kernel_routine.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

t_config *config;
t_log *logger;
pthread_mutex_t mutexLogger;
pthread_mutex_t mx_main;

void *crearServidorMemoria(void *);
void *atenderConexion(void *);

#endif /* MEMORIA_H_ */