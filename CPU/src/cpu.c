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



int main(){
    t_config *config;
    config = config_create("../cpu.config");
    printf("%s\n", config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
    t_log *logger = log_create("./cpu.log", "CPU - Main", 0, LOG_LEVEL_INFO);

    pthread_t interrupt, dispatch, executer;
    if (pthread_create(&interrupt, NULL, interrupt_server, (void *) config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }

    if (pthread_create(&dispatch, NULL, dispatch_server, (void *) config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }
    
    if (pthread_create(&executer, NULL,ciclo_instruccion, (void *) config) < 0)
    {
        perror("Error: ciclo instruccion thread failed."); // cambiar a los logs
    }

    sem_init(&sem, 1, 0);
    sem_wait(&sem);
    finalizar_cpu(config,logger);

    return 0;
}

void* ciclo_instruccion(void* config){//hace falta el config?
    pthread_mutex_lock(&mutex_ejecucion);
    ////////////// FETCH //////////////
    char* instruccion;
    {
        instruccion = mi_contexto->instrucciones[mi_contexto->program_counter];
        mi_contexto->program_counter++;
    }
    ////////////// DECODE //////////////
    //AIUDA
    {

    }
    ////////////// EXECUTE //////////////
    {

    }
    ////////////// CHECK INTERRUPT //////////////
    {
        
    }
}

void* interrupt_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_INTERRUPT");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de interrupt.\n");
    iniciar_servidor(ipCPU,puertoServidor,interrupt_routine);
}

void* dispatch_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_DISPATCH");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de dispatch.\n");
    iniciar_servidor(ipCPU,puertoServidor,dispatch_routine);
}


void *dispatch_routine(void* socket){
    //logica consumidor - productor, semaforo que se libera por kernel
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    ////////////// DECODING PARAM //////////////
    int socket_dispatch = *(int *)socket;
    op_code codigo_operacion;
    
    
    int __attribute__((unused)) tamanio_paquete, tamanio_restante;
    void* msg = NULL;
    t_paquete* paquete;
    
    ////////////// CREANDO LOGGER DEL HANDLER //////////////
    t_log *logger;
    {
        char buffer[25];
        sprintf(buffer, "Cpu - Kernel Dispatch");
        logger = log_create("./kernel.log", buffer, 0, LOG_LEVEL_INFO);
    }
    while(1){
        codigo_operacion = recibir_operacion(socket_dispatch);
        tamanio_paquete = tamanio_restante = largo_paquete(socket_dispatch);

        

        if(codigo_operacion != CONTEXTO){
            perror("Error: Conexion con dispatch no recibio un contexto de ejecucion");
            logger_monitor_error(logger, "Error: Inicio de kernel dispatch con codigo inesperado. Finalizando hilo.");
            exit(EXIT_FAILURE);
        }

        uint32_t id;
        msg = recibir(socket_dispatch);
        id = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        ////////////// Recibiendo PC //////////////
        uint32_t pc;
        msg = recibir(socket_dispatch);
        pc = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        ////////////// Recibiendo Registros //////////////
        uint32_t registros[4];
        tamanio_restante -= 8;
        {
            for(size_t i = 0; i<4; i++){
                msg = recibir(socket_dispatch);
                registros[i] = *((uint32_t *)msg);
                free(msg);
                msg = NULL;
            }
            tamanio_restante -=32;
        }
        ////////////// Recibiendo Segmentos //////////////
        //ayuda
        ////////////// Recibiendo Instrucciones//////////////
        char **instrucciones;
        uint32_t cantidad = 0;
        //uint32_t cantidad = 0;
        {
            while(tamanio_restante > 0)
            {
                msg = recibir(socket_dispatch);
                instrucciones = (char **)realloc(instrucciones, sizeof(char *) * (cantidad + 1));
                instrucciones[cantidad] = msg;
                msg = NULL;

                tamanio_restante -= (strlen(instrucciones[cantidad]) + 1 + sizeof(int));
                cantidad++;
            }
        }
        ////////////// Creando contexto //////////////
        
        {
            mi_contexto->id = id;
            mi_contexto->program_counter = pc;
            mi_contexto->instrucciones = instrucciones;
            for(size_t i = 0;i<4;i++){
                mi_contexto->registros[i] = registros[i];
            }
        }

        pthread_mutex_unlock(&mutex_ejecucion);
        ////////////// Esperando para enviar contexto //////////////
        pthread_mutex_lock(&mutex_dispatch_response);//este mutex devuelve el contexto
        logger_cpu_info(logger,"realizando envio contexto");
        paquete = crear_paquete(CONTEXTO);
        agregar_a_paquete(paquete,(void *)mi_contexto->id,sizeof(uint32_t));
        agregar_a_paquete(paquete,(void *)mi_contexto->program_counter,sizeof(uint32_t));
        for(size_t i = 0; i<4;i++)
        {
            agregar_a_paquete(paquete,(void *)mi_contexto->registros,sizeof(uint32_t));
        }
        for(size_t i = 0;i<cantidad;i++){
            agregar_a_paquete(paquete,(void *)mi_contexto->instrucciones[i],strlen(mi_contexto->instrucciones[i])+1);
        }
        enviar_paquete(paquete,socket_dispatch);
        eliminar_paquete(paquete);
    }
        
    pthread_exit(return_status);
}

void *interrupt_routine(void* socket){
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;

    int socket_cliente = *(int *)socket;
    op_code codigo_operacion;
    int tamaño_paquete;
    void* msg;
    t_paquete* paquete;

    while (1)
    {
        codigo_operacion = recibir_operacion(socket_cliente);
        tamaño_paquete = largo_paquete(socket_cliente);

        switch(codigo_operacion) {
            case MENSAJE:
                msg = recibir(socket_cliente);
                printf("Recibi el mensaje: %s\nEn el socket: %d\n", (char*) msg, socket_cliente);

                char* respuesta = "Recibido";
                paquete = crear_paquete(RESPUESTA);
                agregar_a_paquete(paquete, (void*) respuesta, strlen(respuesta) + 1);
                enviar_paquete(paquete, socket_cliente);
                eliminar_paquete(paquete);

                free(msg);
                break;
            default:
                perror("Recibí una operacion inesperada. Terminando programa.\n");
                *return_status = 1;
                pthread_exit(return_status);
                break;          
        }
    }
    pthread_exit(return_status);
}
