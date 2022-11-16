#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "cpu_utils.h"
#include "globals.h"

void init_globals_cpu(t_config *config)
{
    int entradasTLB = config_get_int_value((t_config*) config, "ENTRADAS_TLB");

    pthread_mutex_init(&mutex_logger, NULL);
    pthread_mutex_init(&mutex_dispatch_response, NULL);
    pthread_mutex_init(&mutex_ejecucion, NULL);
    pthread_mutex_init(&mutex_flag, NULL);
    sem_init(&sem_ciclo_instruccion,0,0);
    sem_init(&sem_envio_contexto,0,0);
    sem_init(&sem_conexion_memoria,0,0);
    sem_init(&sem_mmu,0,0);
    mi_contexto=(t_contexto*)malloc(sizeof(t_contexto));
    configMemoria=(t_configMemoria*)malloc(sizeof(t_configMemoria));
    configMemoria->entradasTLB = entradasTLB;
    tlb=(t_tlb*)malloc(sizeof(t_tlb)*(entradasTLB));
    for(uint i = 0; i < entradasTLB; i++){
        tlb[i].pid = -1;
    }
}

void finalizar_cpu(t_config *config, t_log *logger)
{
    //finalizar_algoritmo();
    pthread_mutex_destroy(&mutex_logger);
    pthread_mutex_destroy(&mutex_dispatch_response);
    pthread_mutex_destroy(&mutex_ejecucion);
    pthread_mutex_destroy(&mutex_flag);
    sem_close(&sem_ciclo_instruccion);
    sem_close(&sem_envio_contexto);
    sem_close(&sem_conexion_memoria);
    sem_close(&sem_mmu);
    free(mi_contexto);
    free(configMemoria);
    free(tlb);
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
