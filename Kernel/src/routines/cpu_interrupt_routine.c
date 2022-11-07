#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "cpu_interrupt_routine.h"
#include "../globals.h"
#include "../kernel_utils.h"

void *cpu_interrupt_routine(void *config)
{
    int socket;
    {
        char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_CPU_INTERRUPT");
        char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
        socket = crear_conexion(ipCPU,puertoServidor);
    }
    
    t_log* logger_interrupt;
    {
        logger_interrupt= log_create("../kernel.log", "Kernel - Interrupt", false, LOG_LEVEL_INFO);
    }

    t_paquete *paquete;
    uint32_t local_pid_to_interrupt;
    char* msg = NULL;
    
    while(1)
    {
        sem_wait(&sem_interrupt_routine);

        local_pid_to_interrupt = global_pid_to_interrupt;

        sem_post(&sem_interrupt_algorithms);

        paquete = crear_paquete(DESALOJO_PROCESO);
        agregar_a_paquete(paquete, (void *) &local_pid_to_interrupt, sizeof(uint32_t));
        enviar_paquete(paquete, socket);
        eliminar_paquete(paquete);
        msg = string_from_format("Proceso %lu - FIN QUANTUM", (unsigned long) local_pid_to_interrupt);
        logger_monitor_info(logger_interrupt, msg);
        free(msg);
        msg = NULL;
    }
    
    liberar_conexion(socket);
    log_destroy(logger_interrupt);
    exit(EXIT_SUCCESS);
    return NULL;
}
