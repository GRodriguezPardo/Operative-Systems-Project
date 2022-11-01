#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "cpu_interrupt_routine.h"
#include "../globals.h"

void *cpu_interrupt_routine(void *config)
{
    int socket;
    {
        char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_CPU_INTERRUPT");
        char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
        socket = crear_conexion(ipCPU,puertoServidor);
    }
    
    t_paquete *paquete;
    uint32_t local_pid_to_interrupt;
    
    while(1)
    {
        sem_wait(&sem_interrupt_routine);

        local_pid_to_interrupt = global_pid_to_interrupt;

        sem_post(&sem_interrupt_algorithms);

        paquete = crear_paquete(DESALOJO_PROCESO);
        agregar_a_paquete(paquete, (void *) &local_pid_to_interrupt, sizeof(uint32_t));
        enviar_paquete(paquete, socket);
        eliminar_paquete(paquete);

        printf("\nMensaje enviado.\n");
    }
    
    printf("Terminando programa.\n");
    liberar_conexion(socket);

    exit(EXIT_SUCCESS);
    return NULL;
}
