#include "paginacion.h"

static t_pagina *pag_crearPagina();
static uint32_t frame_asignar_libre(t_bitarray *mapaFrames);
static uint32_t frame_buscar_posicion_libre(t_bitarray *mapaFrames);
static bool frame_esta_asignado(t_bitarray *mapaFrames, int frame);
static void frame_marcar_asignado(t_bitarray *mapaFrames, int marco);

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

void pag_liberarTablasPaginas(uint32_t idProceso){
}

void pag_destruirTablaPaginas(void *_tabla){
    t_tablaPaginas *tablaPag = (t_tablaPaginas *)_tabla;
    list_destroy_and_destroy_elements(tablaPag->tabla, &pag_destruirPagina);
}

void pag_destruirPagina(void *pag){
    free(pag);
}

// FUNCIONES PRIVADAS

static t_pagina *pag_crearPagina(){
    t_pagina *pagina = (t_pagina *) malloc(sizeof(t_pagina));
    pagina->marco = frame_asignar_libre(mapaFrames);
    pagina->modificado = 0;
    pagina->presente = 0;
    pagina->usado = 0;
    pagina->posicion_swap = 2048; //reemplazar por swap_ubicarPagina
    return pagina;
}

static uint32_t frame_asignar_libre(t_bitarray *mapaFrames){
    uint32_t frameLibre = frame_buscar_posicion_libre(mapaFrames);
    frame_marcar_asignado(mapaFrames, frameLibre);
    return frameLibre;
}

static uint32_t frame_buscar_posicion_libre(t_bitarray *mapaFrames){
    uint32_t posLibre = UINT32_MAX;
    for (uint32_t i = 0; i < configMemoria.marcosEnMemoria; i++)
    {
        if(!frame_esta_asignado(mapaFrames, i)){
            posLibre = i;
            break;
        }
    }
    return posLibre;
}

static bool frame_esta_asignado(t_bitarray *mapaFrames, int frame){
    return bitarray_test_bit(mapaFrames, frame);
}

static void frame_marcar_asignado(t_bitarray *mapaFrames, int frame){
    bitarray_set_bit(mapaFrames, frame);
}