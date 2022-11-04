#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "blocked_routine.h"

void* blocked_process_routine(void* param);

void bloquear_proceso(t_pcb* unPcb, char* dispositivo, uint32_t unidades)
{
    void* param = malloc(sizeof(t_pcb*) + sizeof(uint32_t) + strlen(dispositivo) + 1);

    *((t_pcb**) param) = unPcb;
    *((uint32_t*) param + sizeof(t_pcb*)) = unidades;
    memcpy(param + sizeof(t_pcb*) + sizeof(uint32_t), dispositivo, strlen(dispositivo) + 1);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, blocked_process_routine, param);

    return;
}

void* blocked_process_routine(void* param)
{
    t_pcb* unPcb = *((t_pcb**) param);
    uint32_t unidades = *((uint32_t*) param + sizeof(t_pcb*));
    char* dispositivo = param + sizeof(t_pcb*) + sizeof(uint32_t);

    if(!strcmp(dispositivo, "PANTALLA"))
    {
        sem_wait(&(sem_dispositivos_IO[0]));
        {
            unPcb -> pipeline.operacion = CONSOLE_OUTPUT;
            unPcb -> pipeline.valor = (unPcb -> registros)[unidades];

            sem_post(&(unPcb -> console_semaphore));
            sem_wait(&(unPcb -> console_waiter_semaphore));

            if(unPcb -> pipeline.operacion != CONSOLE_OUTPUT_RESPUESTA){
                perror("ERROR INSEPERADO AL RECIBIR CONSOLE OUTPUT EN CONSOLA");
                exit(EXIT_FAILURE);
            }
        }
        sem_post(&(sem_dispositivos_IO[0]));
    } else if(!strcmp(dispositivo, "TECLADO")) {
        sem_wait(&(sem_dispositivos_IO[1]));
        {
            unPcb -> pipeline.operacion = CONSOLE_INPUT;

            sem_post(&(unPcb -> console_semaphore));
            sem_wait(&(unPcb -> console_waiter_semaphore));

            if(unPcb -> pipeline.operacion != CONSOLE_INPUT_RESPUESTA){
                perror("ERROR INSEPERADO AL RECIBIRI CONSOLE INPUT EN CONSOLA");
                exit(EXIT_FAILURE);
            }

            unPcb -> pipeline.valor = (unPcb -> registros)[unidades];
        }
        sem_post(&(sem_dispositivos_IO[1]));
    } else {
        sem_t* sem_dispositivo = NULL;
        size_t num_dispositivo = 0;
        for (size_t i = 0; i < cantidad_dispositivos_IO; i++)
        {
            if(!strcmp(dispositivo, dispositivos_IO[i]))
            {
                sem_dispositivo = &(sem_dispositivos_IO[i+2]);
                num_dispositivo = i;
                break;
            }
        }
        if(sem_dispositivo == NULL)
        {
            perror("DISPOSITIVO I/O DESCONOCIDO, FINALIZANDO CON ERROR");
            exit(EXIT_FAILURE);
        }
        
        sem_wait(sem_dispositivo);
        usleep(tiempos_IO[num_dispositivo] * unidades * 1000);
        sem_post(sem_dispositivo);
    }

    ingresar_a_ready(unPcb, BLOQUEO_PROCESO);
    sem_post(&sem_proceso_entro_a_ready);

    free(param);
    return NULL;
}