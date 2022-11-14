#ifndef MMU_H
#define MMU_H

#include "../globals.h"
#include "../cpu_utils.h"

bool buscarEnTLB(uint32_t num_segmento, uint32_t num_pagina, uint32_t desplazamiento_pag, op_code instruccion);
uint32_t calcularDirecFisica(uint32_t marco, uint32_t desplazamiento_pag);
void buscarMarco(uint32_t nroSegmento, uint32_t idPagina);
op_code traducciones(op_code instruccion);



#endif