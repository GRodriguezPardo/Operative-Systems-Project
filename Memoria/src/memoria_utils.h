#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include "swap.h"
#include <thesenate/tcp_serializacion.h>
#include <stdlib.h>
#include <string.h>

void esperar_hilos();
void finalizar_memoria();

void loggear_info(t_log *, char *);
void loggear_error(t_log *, char *);

//Atiende el handshake del cliente.
void recibirHandshake(int, op_code);
#endif //MEMORIA_UTILS_H_