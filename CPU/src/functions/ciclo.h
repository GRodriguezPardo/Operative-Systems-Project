#ifndef CICLO_H_
#define CICLO_H_

#include "../globals.h"
#include "../cpu_utils.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

void *ciclo_instruccion(void *config);



#endif