#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_server.h>
#include <thesenate/tcp_client.h>
#include <unistd.h>
#include "kernel.h"
#include "kernel_utils.h"
#include "routines/consola_routine.h"
#include "routines/cpu_dispatch_routine.h"
#include "routines/cpu_interrupt_routine.h"
#include "routines/planificador_routine.h"
#include "routines/memoria_routine.h"
#include "globals.h"

int main(int argc, char** argv)
{
    pthread_t memoria, cpu_dispatch, cpu_interrupt, planificador_largo;

    ////////////// INICIANDO CONFIG Y LOGGER //////////////
    t_config *config = config_create("../kernel.config");
    t_log *logger = log_create("../kernel.log", "Kernel - Main", 0, LOG_LEVEL_INFO);

    ////////////// INICIANDO VARIABLES GLOBALES //////////////
    int grado_multiprogramacion = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");
    init_globals_kernel(grado_multiprogramacion);

    logger_monitor_info(logger, "Iniciando Kernel.");

    ////////////// CONFIGURACION DEL ALGORTIMO //////////////
    char* algoritmo = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
    if(configurar_algoritmo(algoritmo))
    {
        perror("Error: Algortimo invalido.\n");
        logger_monitor_error(logger, "Error: Algortimo invalido.");
        exit(EXIT_FAILURE);
    };

    ////////////// CREANDO HILOS DE SUB-RUTINAS //////////////
    /* 
    logger_monitor_info(logger, "Conectando a memoria.");
    if (pthread_create(&memoria, NULL, memoria_routine, (void *) config) < 0)
    {
        perror("Error: Memoria thread failed.\n");
        logger_monitor_error(logger, "Error: Memoria thread failed.");
    }

    logger_monitor_info(logger, "Conectando a CPU-Dispatch.");
    if (pthread_create(&cpu_dispatch, NULL, cpu_dispatch_routine, (void *) config) < 0)
    {
        perror("Error: CPU-Dispatch thread failed.\n");
        logger_monitor_error(logger, "Error: CPU-Dispatch thread failed.");
    }

    logger_monitor_info(logger, "Conectando a CPU-Interrupt.");
    if (pthread_create(&cpu_interrupt, NULL, cpu_interrupt_routine, (void *) config) < 0)
    {
        perror("Error: CPU-Interrupt thread failed.\n");
        logger_monitor_error(logger, "Error: CPU-Interrupt thread failed.");
    }
    */

    logger_monitor_info(logger, "Iniciando planificador de largo plazo.");
    if (pthread_create(&planificador_largo, NULL, new_a_ready, NULL) < 0)
    {
        perror("Error: Long-term planner thread failed.\n");
        logger_monitor_error(logger, "Error: Long-term planner thread failed.");
    } 
    
    ////////////// CREANDO SERVER DE CONSOLAS //////////////
    char* puertoServidor = config_get_string_value(config, "PUERTO_ESCUCHA");
    char* ipKernel = config_get_string_value(config,"IP_KERNEL");
    
    logger_monitor_info(logger, "Iniciando servidor de consolas.");
    iniciar_servidor_consolas(ipKernel, puertoServidor, consola_routine, &console_acumulator, &console_status);

    ////////////// FINALIZANDO KERNEL //////////////
    logger_monitor_info(logger, "Finalizando Kernel.");
    finalizar_kernel(config, logger);

    return EXIT_SUCCESS;
}
