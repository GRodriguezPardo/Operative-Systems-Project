#ifndef CPUROUTINE_H_
#define CPUROUTINE_H_

#include "../memoria_utils.h"
#include "../memoria_globals.h"
#include "../paginacion.h"
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <thesenate/tcp_serializacion.h>

void cpu_routine(int, int*);

uint32_t leer_memoria(uint32_t direccionFisica);
void escribir_memoria(uint32_t direccionFisica, uint32_t valor);
void responder_cpu(int socket, op_code code, uint32_t valor);
void responder_handshakeCPU(int socket);

void marcarPaginaUsada(uint32_t direccion, bool fueModificada);

#endif /* CPUROUTINE_H_ */