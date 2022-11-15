#ifndef PAGINACION_H_
#define PAGINACION_H_

#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <stdint.h>
#include "memoria_utils.h"
#include "memoria_globals.h"

//Crea una tabla y devuelve su id
uint32_t pag_crearTablaPaginas(uint32_t idProceso, uint32_t tamanioSegmento);
void pag_liberarTablasProceso(uint32_t idProceso);
int pag_obtenerMarcoPagina(uint32_t idTablaPaginas, uint32_t idPagina, uint32_t *marco);
void pag_destruirTablaPaginas(void *tabla);
void pag_destruirPagina(void *pag);
uint32_t frame_asignar_libre(t_bitarray *mapaFrames);

#endif /* PAGINACION_H_ */