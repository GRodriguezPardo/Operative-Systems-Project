#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_server.h>
#include "cpu.h"



int main(){
    t_config *config;
    config = config_create("cpu.config");

     pthread_t interrupt, dispatch;
    if (pthread_create(&interrupt, NULL, interrupt_server, (void *)&config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }

    if (pthread_create(&dispatch, NULL, dispatch_server, (void *)&config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }

    return 0;
}

void* interrupt_server(void* config){
    char* puertoServidor = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    char* ipCPU = config_get_string_value(config,"IP_CPU");
    iniciar_servidor(ipCPU,puertoServidor,interrupt_routine);
}

void* dispatch_server(void* config){
    char* puertoServidor = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    char* ipCPU = config_get_string_value(config,"IP_CPU");
    iniciar_servidor(ipCPU,puertoServidor,dispatch_routine);
}


void *dispatch_routine(void* socket){
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
