#include "paginacion.h"

static t_tablaPaginas_pag *pag_crearPagina();
static void pag_destruirPagina(t_tablaPaginas_pag *);

t_list *pag_crearTablaPaginas(uint32_t tamanioSegmento){
    t_list *tablaPaginas = list_create();

    for (uint32_t i = 0; i < configMemoria.entradasPorTabla; i++)
    {
        t_tablaPaginas_pag *pagina = pag_crearPagina();
        list_add(tablaPaginas, pagina);
    }

    return tablaPaginas;
}

int pag_obtenerMarcoPagina(uint32_t idTablaPaginas, uint32_t idPagina, uint32_t *marco){
    int retVal = 0;
    pthread_mutex_lock(&mx_espacioKernel);
    t_list *tabla = list_get(espacioKernel, idTablaPaginas);
    pthread_mutex_unlock(&mx_espacioKernel);

    t_tablaPaginas_pag *pagina = list_get(tabla, idPagina);

    if (pagina->presente)
        *marco = pagina->marco;
    else
        retVal = -1;
    
    return retVal;
}

void pag_destruirTablaPaginas(t_list *tablaPag){
    list_destroy_and_destroy_elements(tablaPag, &pag_destruirPagina);
}

// FUNCIONES PRIVADAS

static t_tablaPaginas_pag *pag_crearPagina(){
    t_tablaPaginas_pag *pagina = (t_tablaPaginas_pag *) malloc(sizeof(t_tablaPaginas_pag));
    pagina->marco = 4; //reemplazar por encontrar_frame_libre
    pagina->modificado = 0;
    pagina->presente = 0;
    pagina->usado = 0;
    pagina->posicion_swap = 2048; //reemplazar por swap_ubicarPagina
    return pagina;
}

static void pag_destruirPagina(t_tablaPaginas_pag *pag){
    free(pag);
}