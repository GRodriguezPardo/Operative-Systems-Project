#include "paginacion.h"

static t_pagina *pag_crearPagina(int id, uint32_t numeroSegmento, bool estaEnSwap);
static void pag_destruir_pagina(void *_pagina);

uint32_t pag_crearTablaPaginas(t_list *listaTablas, uint32_t tamanioSegmento){
    t_list *nuevaTabla = list_create();

    uint32_t idTabla = listaTablas->elements_count; //siempre de 0 a cantidadDeSegmentos    
    uint32_t cantRealPaginas = tamanioSegmento / ConfigMemoria.tamanioPagina;
    for (uint32_t i = 0; i < ConfigMemoria.paginasPorTabla; i++)
    {
        t_pagina *pagina = pag_crearPagina(nuevaTabla->elements_count, idTabla, i < cantRealPaginas);
        list_add(nuevaTabla, pagina);
    }

    list_add(listaTablas, nuevaTabla);
    return idTabla;
}

/* Obtiene el marco correspondiente a la pagina especificada. 
Si la página está presente en memoria, coloca el # de marco en MARCO y retorna 0.
En caso contrario, retorna -1.
*/
int pag_obtenerMarcoPagina(uint32_t pid, uint32_t idTabla, uint32_t numPagina, uint32_t *marco){
    int retVal = 0;
    t_pagina *pagina = get_pagina(pid, idTabla, numPagina);

    if (pagina->presente)
        *marco = pagina->marco;
    else
        retVal = -1;
    
    return retVal;
}

void pag_destruirTablaPaginas(void *_tabla){
    t_list *tablaPag = (t_list *)_tabla;
    list_destroy_and_destroy_elements(tablaPag, &pag_destruir_pagina);
}

// FUNCIONES PRIVADAS

static t_pagina *pag_crearPagina(int id, uint32_t _numeroSegmento, bool estaEnSwap){
    t_pagina *pagina = (t_pagina *) malloc(sizeof(t_pagina));
    pagina->id = (uint32_t)id;
    pagina->numeroSegmento = _numeroSegmento;
    pagina->marco = UINT32_MAX;
    pagina->modificado = false;
    pagina->presente = false;
    pagina->usado = false;
    pagina->posicion_swap = estaEnSwap ? swap_crear_pagina() : UINT32_MAX;

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