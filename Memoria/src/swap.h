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
uint32_t swap_in(uint32_t pid, t_pagina *nuevaPagina, uint32_t numMarco);
void swap_out(uint32_t pid, t_pagina *pagina);
void* swap_leer_pagina(int fd, uint32_t posicionPrimerByte);
void swap_escribir_pagina(int fd, uint32_t posicionPrimerByte, void *inicioPagina);

uint32_t reemplazarPagina(uint32_t pid, t_pagina *paginaReferida);
t_pagina *seleccionarV_Clock(t_queue *qPaginasPresentes);
t_pagina *seleccionarV_ClockMejorado(t_queue *qPaginasPresentes);

//auxiliares
bool memoria_esta_llena();
bool proceso_alcanzo_max_marcos(uint32_t pid);

#endif //SWAP_H_