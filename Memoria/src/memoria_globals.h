#ifndef MEMORIA_GLOBALS_H_
#define MEMORIA_GLOBALS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_server.h>
#include <thesenate/tcp_serializacion.h>
#include <pthread.h>

//////// VARIABLES GLOBALES /////////

extern t_config *config;
extern t_log *logger;
extern pthread_mutex_t mx_logger;
extern pthread_mutex_t mx_main;
extern pthread_mutex_t mx_memoria;
extern void *memoriaPrincipal;
extern int tamanioMaxSegmento;
extern int cantMaxMarcosXProceso;

extern int swapFd;

extern void (*reemplazarPagina)();

#endif /* MEMORIA_GLOBALS_H_ */