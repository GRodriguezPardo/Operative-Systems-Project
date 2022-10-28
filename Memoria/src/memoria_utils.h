#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include "swap.h"
#include <thesenate/tcp_serializacion.h>
#include <stdlib.h>
#include <string.h>

void memoria_iniciar();
void esperarHilos();
void memoria_finalizar();

//Atiende y responde al handshake del cliente.
void resolverHandshake(int, op_code);
//Responde al handshake del cliente.
#endif //MEMORIA_UTILS_H_