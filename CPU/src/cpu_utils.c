#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu_utils.h"
#include "globals.h"

void init_globals_cpu()
{

    pthread_mutex_init(&mutex_logger, NULL);

}

void finalizar_cpu(t_config *config, t_log *logger)
{
    finalizar_algoritmo();
    pthread_mutex_destroy(&mutex_logger);
    log_info(logger, "Finalizando programa.");
    log_destroy(logger);
    config_destroy(config);
}

void logger_cpu_info(t_log *logger, const char *message)
{
    pthread_mutex_lock(&mutex_logger);
    log_info(logger, message);
    pthread_mutex_unlock(&mutex_logger);
}


void logger_cpu_error(t_log *logger, const char *message)
{
    pthread_mutex_lock(&mutex_logger);
    log_error(logger, message);
    pthread_mutex_unlock(&mutex_logger);
}
