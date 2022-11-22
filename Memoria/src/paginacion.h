#ifndef PAGINACION_H_
#define PAGINACION_H_

#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <stdint.h>
#include "memoria_utils.h"
#include "memoria_globals.h"
#include "swap.h"

//Crea una tabla y devuelve su id
uint32_t pag_crearTablaPaginas(t_list *listaTablas, uint32_t tamanioSegmento);
void pag_liberarPagina(t_pagina *pag);
int pag_obtenerMarcoPagina(uint32_t pid, uint32_t idTablaPaginas, uint32_t idPagina, uint32_t *marco);
void pag_destruirTablaPaginas(void *tabla);

#endif /* PAGINACION_H_ */