#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdint.h>
#include "memoria_utils.h"

typedef struct tablaPaginas_pag
{
    uint32_t marco,
        presente, 
        usado, 
        modificado,
        posicion_swap;
} t_tablaPaginas_pag;

t_list *pag_crearTablaPaginas(uint32_t tamanioSegmento);
void pag_destruirTablaPaginas(t_list *tabla);
uint32_t pag_obtenerMarcoPagina(uint32_t idTabla, uint32_t numPagina);