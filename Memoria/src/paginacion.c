#include "paginacion.h"

static t_pagina *pag_crearPagina(bool disponibleEnSwap);

uint32_t pag_crearTablaPaginas(uint32_t _idProceso, uint32_t tamSegmento){
    uint32_t idTabla;

    t_tablaPaginas *tablaPaginas = (t_tablaPaginas *) malloc(sizeof(t_tablaPaginas));
    tablaPaginas->idProceso = _idProceso;
    tablaPaginas->tabla = list_create();

    uint32_t cantRealPaginas = tamSegmento / ConfigMemoria.tamanioPagina;
    for (uint32_t indice = 0; indice < ConfigMemoria.paginasPorTabla; indice++)
    {
        bool paginaDisponibleEnSwap = indice < cantRealPaginas;
        t_pagina *pagina = pag_crearPagina(paginaDisponibleEnSwap);
        list_add(tablaPaginas->tabla, pagina);
    }

    pthread_mutex_lock(&mx_espacioTablasPag);
    list_add(EspacioTablasPag, tablaPaginas);
    idTabla = (uint32_t) (EspacioTablasPag->elements_count - 1);
    pthread_mutex_unlock(&mx_espacioTablasPag);

    return idTabla;
}

/* Obtiene el marco correspondiente a la pagina especificada. 
Si la página está presente en memoria, coloca el # de marco en MARCO y retorna 0.
En caso contrario, retorna -1.
*/
int pag_obtenerMarcoPagina(uint32_t idTablaPaginas, uint32_t idPagina, uint32_t *marco){
    int retVal = 0;
    pthread_mutex_lock(&mx_espacioTablasPag);
    t_tablaPaginas *tablaPag = (t_tablaPaginas *)list_get(EspacioTablasPag, idTablaPaginas);
    pthread_mutex_unlock(&mx_espacioTablasPag);

    t_pagina *pagina = list_get(tablaPag->tabla, idPagina);

    if (pagina->presente)
        *marco = pagina->marco;
    else
        retVal = -1;
    
    return retVal;
}

void pag_liberarTablasProceso(uint32_t idProceso){
}

void pag_destruirTablaPaginas(void *_tabla){
    t_tablaPaginas *tablaPag = (t_tablaPaginas *)_tabla;
    list_destroy_and_destroy_elements(tablaPag->tabla, &pag_destruirPagina);
    free(tablaPag);
}

void pag_destruirPagina(void *pag){
    free(pag);
}

// FUNCIONES PRIVADAS

static t_pagina *pag_crearPagina(bool disponibleEnSwap){
    t_pagina *pagina = (t_pagina *) malloc(sizeof(t_pagina));
    pagina->marco = NULL;
    pagina->modificado = false;
    pagina->presente = false;
    pagina->usado = false;
    if(disponibleEnSwap)
        pagina->posicion_swap = 2048; //reemplazar por swap_ubicarPagina
    else 
        pagina->posicion_swap = NULL;

    return pagina;
}