#ifndef PAGINACION_H_
#define PAGINACION_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdint.h>
#include "memoria_utils.h"

typedef struct tablaPaginas
{
    uint32_t idProceso;
    t_list *tabla;
} t_tablaPaginas;

typedef struct pagina
{
    uint32_t marco,
        presente, 
        usado, 
        modificado,
        posicion_swap;
} t_pagina;

//Crea una tabla y devuelve su id
uint32_t pag_crearTablaPaginas(uint32_t idProceso);
void pag_destruirTablaPaginas(t_tablaPaginas *tabla);
int pag_obtenerMarcoPagina(uint32_t idTablaPaginas, uint32_t idPagina, uint32_t *marco);

#endif /* PAGINACION_H_ */