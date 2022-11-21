#ifndef TLB_H_
#define TLB_H_

#include "../globals.h"
#include "../cpu_utils.h"
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
typedef struct t_tlb {
    uint32_t pid;
    uint32_t nro_segmento;
    uint32_t nro_pag;
    uint32_t marco;
    uint32_t nro_entrada;
    bool presente;
} t_tlb;


bool condicionPID(void *tlb);
void limpiar_tlb();
void reemplazar_pagina(t_tlb *entrada);
int buscarEnTLB(uint32_t pid, uint32_t num_segmento, uint32_t num_pagina);
void agregar_entrada_tlb(uint32_t pid, uint32_t num_segmento, uint32_t num_pagina, uint32_t num_marco);
void iniciar_estructuras();

#endif