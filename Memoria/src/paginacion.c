#include "paginacion.h"

static t_pagina *pag_crearPagina(int id, uint32_t numeroSegmento);
static void pag_destruir_pagina(void *_pagina);

uint32_t pag_crearTablaPaginas(t_list *listaTablas, uint32_t numSegmento){
    t_list *tabla = list_create();

    uint32_t idTabla = listaTablas->elements_count;
    for (uint32_t i = 0; i < ConfigMemoria.paginasPorTabla; i++)
    {
        t_pagina *pagina = pag_crearPagina(tabla->elements_count, numSegmento);
        list_add(tabla, pagina);
    }

    list_add(listaTablas, tabla);
    return idTabla;
}

/* Obtiene el marco correspondiente a la pagina especificada. 
Si la página está presente en memoria, coloca el # de marco en MARCO y retorna 0.
En caso contrario, retorna -1.
*/
int pag_obtenerMarcoPagina(uint32_t pid, uint32_t idTabla, uint32_t numPagina, uint32_t *marco){
    int retVal = 0;
    t_pagina *pagina = pag_get_pagina(pid, idTabla, numPagina);

    if (pagina->presente)
        *marco = pagina->marco;
    else
        retVal = -1;
    
    return retVal;
}

t_pagina *pag_get_pagina(uint32_t pid, uint32_t idTabla, uint32_t numPagina){
    t_dataProceso *dataP = get_data_proceso(pid);
    t_list *tablaActual = list_get(dataP->tablasProceso, idTabla);
    t_pagina *pagina = list_get(tablaActual, numPagina);
    return pagina;

}

void pag_destruirTablaPaginas(void *_tabla){
    t_list *tablaPag = (t_list *)_tabla;
    list_destroy_and_destroy_elements(tablaPag, &free);
}

// FUNCIONES PRIVADAS

static t_pagina *pag_crearPagina(int id, uint32_t _numeroSegmento){
    t_pagina *pagina = (t_pagina *) malloc(sizeof(t_pagina));
    pagina->id = (uint32_t)id;
    pagina->numeroSegmento = _numeroSegmento;
    pagina->marco = NULL;
    pagina->modificado = false;
    pagina->presente = false;
    pagina->usado = false;
    pagina->posicion_swap = swap_crear_pagina();

    return pagina;
}

static void pag_destruir_pagina(void *_pagina){
    t_pagina *paginaActual = (t_pagina *) _pagina;
    if(paginaActual->presente){
        liberar_posicion(MapaFrames, paginaActual->marco);
        borrarEntradaTablaFrames(paginaActual->marco);
    }

    uint32_t numBloque = paginaActual->posicion_swap / ConfigMemoria.tamanioPagina;
    liberar_posicion(MapaSwap, numBloque);
    free(paginaActual);
}