#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_client.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "../globals.h"
#include "../cpu_utils.h"


void *memoria_routine(void*);
uint32_t recibir_uint32t(int socket);
void realizarHandshake(int,t_log*);

#endif /* MEMORIA_H_ */