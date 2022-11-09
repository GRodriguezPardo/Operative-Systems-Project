#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_client.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

typedef struct _configMem
{
    char *ip;
    char *puerto;
    uint32_t entradasTablaPaginas;
    uint32_t tamanioPagina;
    uint32_t tamanioMaximoSegmento;
} t_configMemoria;


void *memoria_com(void*);
uint32_t recibir_uint32t(int socket);
#endif /* MEMORIA_H_ */