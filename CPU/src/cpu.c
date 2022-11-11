#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_server.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include "cpu_utils.h"
#include "cpu.h"
#include "./functions/ciclo.h"
#include "./functions/dispatch.h"
#include "./functions/interrupt.h"
#include "./functions/memoria.h"

sem_t sem,sem_ciclo_instruccion,sem_envio_contexto;

int main(){
    t_config *config;
    config = config_create("../cpu.config");
    printf("%s\n", config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
    t_log *logger = log_create("../cpu.log", "CPU - Main", 0, LOG_LEVEL_INFO);
    init_globals_cpu();
    pthread_t interrupt, dispatch, executer, memoria;
    if (pthread_create(&interrupt, NULL, interrupt_server, (void *) config) < 0)
    {
        perror("Error: Interrupt thread failed."); // cambiar a los logs
    }

    if (pthread_create(&dispatch, NULL, dispatch_server, (void *) config) < 0)
    {
        perror("Error: Dispatch thread failed."); // cambiar a los logs
    }
    
    if (pthread_create(&executer, NULL,ciclo_instruccion, (void *) config) < 0)
    {
        perror("Error: Ciclo instruccion thread failed."); // cambiar a los logs
    }

    if (pthread_create(&memoria, NULL,memoria_routine, (void *) config) < 0)
    {
        perror("Error: Memoria routine thread failed."); // cambiar a los logs
    }


    sem_init(&sem, 1, 0);
    sem_wait(&sem);
    finalizar_cpu(config,logger);

    return 0;
}


