#ifndef MEMORIA_ROUTINE_H_
#define MEMORIA_ROUTINE_H_

#include <commons/config.h>
#include <commons/string.h>
#include <stdio.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>

void *memoria_routine(void *config);
static int conectarAMemoria(t_config *);
static void iniciarHandshake(int);

#endif /* MEMORIA_ROUTINE_H_ */