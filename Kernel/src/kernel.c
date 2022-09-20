#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_server.h>
#include <thesenate/tcp_client.h>
#include "kernel.h"
#include "routines/consola_routine.h"
#include "routines/cpu_dispatch_routine.h"
#include "routines/cpu_interrupt_routine.h"
#include "routines/memoria_routine.h"

int main()
{
    pthread_t memoria, cpu_dispatch, cpu_interrupt;

    t_config *config;
    config = config_create("../kernel.config");

    if (pthread_create(&memoria, NULL, memoria_routine, (void *) config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }

    if (pthread_create(&cpu_dispatch, NULL, cpu_dispatch_routine, (void *) config) < 0)
    {
        perror("Error: Cpu_dispatch thread failed."); // cambiar a los logs
    }

    if (pthread_create(&cpu_interrupt, NULL, cpu_interrupt_routine, (void *) config) < 0)
    {
        perror("Error: Cpu_interrupt thread failed.");
    }
    char* puertoServidor = config_get_string_value(config, "PUERTO_ESCUCHA");
    char* ipKernel = config_get_string_value(config,"IP_KERNEL");
    
    iniciar_servidor(ipKernel, puertoServidor, consola_routine);

    return 0;
}
