#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include "swap.h"
#include <commons/string.h>
#include <thesenate/tcp_serializacion.h>
#include <string.h>

void esperar_hilos();

void loggear_info(t_log *, char *);
void loggear_error(t_log *, char *);
int sonIguales(const char* str1, const char* str2);

void recibir_handshake(int socket, op_code codigo);

void aplicar_retardo(uint32_t tiempo_ms);
uint32_t recibir_uint32t(int socket);

t_list *get_tablas_proceso(uint32_t idProceso);
uint32_t asignar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa);

#endif //MEMORIA_UTILS_H_