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

int main()
{
    pthread_t memoria, cpu_dispatch, cpu_interrupt;

    t_config *config;
    config = config_create("kernel.config");

    if (pthread_create(&memoria, NULL, memoria_routine, (void *)&config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }

    if (pthread_create(&cpu_dispatch, NULL, cpu_dispatch_routine, (void *)&config) < 0)
    {
        perror("Error: Cpu_dispatch thread failed."); // cambiar a los logs
    }

    if (pthread_create(&cpu_interrupt, NULL, cpu_interrupt_routine, (void *)&config) < 0)
    {
        perror("Error: Cpu_interrupt thread failed.");
    }
    char* puertoServidor = config_get_string_value(config, "PUERTO_ESCUCHA");
    char* ipKernel = config_get_string_value(config,"IP_KERNEL");
    
    iniciar_servidor(ipKernel, puertoServidor, consola_routine);

    return 0;
}

void *consola_routine(void *socket)
{   
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

void *memoria_routine(void *config)
{
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    pthread_exit(return_status);
}

void *cpu_dispatch_routine(void *config)
{
    char* puertoServidor = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
    char* ipCPU = config_get_string_value(config,"IP_CPU");
    int socket = crear_conexion(ipCPU,puertoServidor);
    op_code codigo_operacion;
    int tamaño_paquete;
    void *msg;
    char *input = "hola";
    t_paquete *paquete;
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;

    paquete = crear_paquete(MENSAJE);
    agregar_a_paquete(paquete, (void *)input, strlen(input) + 1);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);

    printf("\nMensaje enviado.\nEsperando respuesta...\n");

    codigo_operacion = recibir_operacion(socket);
    tamaño_paquete = largo_paquete(socket);

    switch (codigo_operacion)
    {
        case RESPUESTA:
            msg = recibir(socket);
            printf("Recibi la respuesta: %s\n", (char *)msg);
            free(msg);
            break;
        default:
            perror("Recibí una operacion inesperada. Terminando programa.");
            liberar_conexion(socket);
            *return_status = 1;
            pthread_exit(return_status);
            break;
    }
    
    printf("Terminando programa.\n");
    liberar_conexion(socket);
    pthread_exit(return_status);
}
void *cpu_interrupt_routine(void *config)
{
    char* puertoServidor = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
    char* ipCPU = config_get_string_value(config,"IP_CPU");
    int socket = crear_conexion(ipCPU,puertoServidor);
    op_code codigo_operacion;
    int tamaño_paquete;
    void *msg;
    char *input = "hola";
    t_paquete *paquete;
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;

    paquete = crear_paquete(MENSAJE);
        agregar_a_paquete(paquete, (void *)input, strlen(input) + 1);
        enviar_paquete(paquete, socket);
        eliminar_paquete(paquete);

        printf("\nMensaje enviado.\nEsperando respuesta...\n");

        codigo_operacion = recibir_operacion(socket);
        tamaño_paquete = largo_paquete(socket);

        switch (codigo_operacion)
        {
            case RESPUESTA:
                msg = recibir(socket);
                printf("Recibi la respuesta: %s\n", (char *)msg);
                free(msg);
                break;
            default:
                perror("Recibí una operacion inesperada. Terminando programa.");
                liberar_conexion(socket);
                *return_status = 1;
                pthread_exit(return_status);
                break;
        }
    
    printf("Terminando programa.\n");
    liberar_conexion(socket);
    pthread_exit(return_status);
}
