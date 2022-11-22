#ifndef MEMORIA_UTILS_H_
#define MEMORIA_UTILS_H_

#include "memoria_globals.h"
#include <commons/string.h>
#include <thesenate/tcp_serializacion.h>
#include <string.h>

void esperar_hilos();

void loggear_info(t_log *log, char *mensaje, bool freeMsg);
void loggear_error(t_log *log, char *mensaje, bool freeMsg);
int sonIguales(const char* str1, const char* str2);

void recibir_handshake(int socket, op_code codigo);

void aplicar_retardo(uint32_t tiempo_ms);
uint32_t recibir_uint32t(int socket);

t_infoProceso *get_info_proceso(uint32_t pid);
t_pagina *get_pagina(uint32_t pid, uint32_t idTabla, uint32_t numPagina);

uint32_t asignar_frame_libre();
uint32_t asignar_slot_swap_libre();
uint32_t asignar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa);
void liberar_posicion(t_bitarray *bitmap, uint32_t index);

void crearEntradaTablaFrames(uint32_t numFrame, uint32_t pid, uint32_t idTabla, uint32_t numPagina);
void borrarEntradaTablaFrames(uint32_t numFrame);

#endif //MEMORIA_UTILS_H_