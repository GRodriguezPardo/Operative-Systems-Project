#include "paginacion.h"

static t_pagina *pag_crearPagina();
static void pag_destruirPagina(t_pagina *);

uint32_t pag_crearTablaPaginas(uint32_t _idProceso){
    uint32_t idTabla;
    
    t_tablaPaginas *tablaPaginas = (t_tablaPaginas *) malloc(sizeof(t_tablaPaginas));
    tablaPaginas->idProceso = _idProceso;
    tablaPaginas->tabla = list_create();

    for (uint32_t i = 0; i < configMemoria.entradasPorTabla; i++)
    {
        t_pagina *pagina = pag_crearPagina();
        list_add(tablaPaginas->tabla, pagina);
    }

    pthread_mutex_lock(&mx_espacioTablasPag);
    list_add(espacioTablasPag, tablaPaginas);
    idTabla = (uint32_t) (espacioTablasPag->elements_count - 1);
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
    t_tablaPaginas *tablaPag = (t_tablaPaginas *)list_get(espacioTablasPag, idTablaPaginas);
    pthread_mutex_unlock(&mx_espacioTablasPag);

    t_pagina *pagina = list_get(tablaPag->tabla, idPagina);

    if (pagina->presente)
        *marco = pagina->marco;
    else
        retVal = -1;
    
    return retVal;
}

void pag_destruirTablaPaginas(t_tablaPaginas *tablaPag){
    list_destroy_and_destroy_elements(tablaPag->tabla, &pag_destruirPagina);
}

// FUNCIONES PRIVADAS

static t_pagina *pag_crearPagina(){
    t_pagina *pagina = (t_pagina *) malloc(sizeof(t_pagina));
    pagina->marco = 4; //reemplazar por encontrar_frame_libre
    pagina->modificado = 0;
    pagina->presente = 0;
    pagina->usado = 0;
    pagina->posicion_swap = 2048; //reemplazar por swap_ubicarPagina
    return pagina;
}

static void pag_destruirPagina(t_pagina *pag){
    free(pag);
}