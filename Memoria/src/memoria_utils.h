#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include "swap.h"
#include <commons/string.h>
#include <thesenate/tcp_serializacion.h>
#include <stdlib.h>
#include <string.h>

void esperar_hilos();

void loggear_info(t_log *, char *);
void loggear_error(t_log *, char *);

void recibir_handshake(int socket, op_code codigo);

void aplicar_retardo(uint32_t tiempo_ms);
uint32_t recibir_uint32t(int socket);

t_list *get_tablas_proceso(uint32_t idProceso);

#endif //MEMORIA_UTILS_H_