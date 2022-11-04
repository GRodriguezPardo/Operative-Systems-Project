#ifndef DISPATCH_H_
#define DISPATCH_H_

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
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_server.h>

void *dispatch_server(void *config);
void *dispatch_routine(void *socket);


#endif