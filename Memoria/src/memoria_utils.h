#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include <thesenate/tcp_serializacion.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void inicializarMemoria();
void esperarHilos();
void finalizarMemoria();

//Atiende y responde al handshake del cliente.
void resolverHandshake(int, op_code);
//Responde al handshake del cliente.
#endif