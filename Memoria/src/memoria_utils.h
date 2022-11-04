#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include "swap.h"
#include <thesenate/tcp_serializacion.h>
#include <stdlib.h>
#include <string.h>

void esperar_hilos();

void loggear_info(t_log *, char *);
void loggear_error(t_log *, char *);

void recibir_handshake(int socket, op_code codigo);

void aplicar_retardoMemoria();
void aplicar_retardoSwap();

uint32_t recibir_uint32t(int socket);

#endif //MEMORIA_UTILS_H_