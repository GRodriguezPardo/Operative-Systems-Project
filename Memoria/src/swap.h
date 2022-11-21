#ifndef SWAP_H_
#define SWAP_H_

#include "memoria_globals.h"
#include "memoria_utils.h"
#include "paginacion.h"
#include <fcntl.h>
#include <unistd.h>

void swap_inicializar();
uint32_t swap_resolver_pageFault(uint32_t pid, uint32_t idTabla, uint32_t numPagina);
void swap_cerrar();

uint32_t swap_crear_pagina();
uint32_t swap_in(t_pagina *nuevaPagina, uint32_t numMarco);
void* swap_leer_pagina(int fileDes, uint32_t posicionPrimerByte);
void swap_escribir_pagina(int fd, uint32_t posicionSwap, void *inicioPagina);

uint32_t reemplazar_Clock(uint32_t pid, t_pagina *nuevaPagina);
uint32_t reemplazar_ClockM(uint32_t pid, t_pagina *nuevaPagina);

bool memoria_esta_llena();
bool proceso_alcanzo_max_marcos(uint32_t pid);

#endif //SWAP_H_