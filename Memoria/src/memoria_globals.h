#ifndef MEMORIA_GLOBALS_H_
#define MEMORIA_GLOBALS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_server.h>
#include <thesenate/tcp_serializacion.h>

extern t_config *config;
extern t_log *logger;
extern pthread_mutex_t mutexLogger;
extern pthread_mutex_t mx_main;

#endif /* MEMORIA_GLOBALS_H_ */