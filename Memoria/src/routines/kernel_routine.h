#ifndef KERNELROUTINE_H_
#define KERNELROUTINE_H_

#include "../memoria_utils.h"
#include "../paginacion.h"
#include "../memoria_globals.h"
#include <stdlib.h>
#include <thesenate/tcp_serializacion.h>

void kernel_routine(int socketFd, int *returnStatus);

void crearEntradaTablaFrames(uint32_t numFrame, uint32_t pid, uint32_t idTabla, uint32_t numPagina);
void liberar_proceso(uint32_t pid);
void liberar_pagina(void *pagina);
void borrarEntradaTablaFrames(uint32_t numFrame);

#endif /* KERNELROUTINE_H_ */