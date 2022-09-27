#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "cpu_interrupt_routine.h"

void *cpu_interrupt_routine(void *config)
{
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_CPU_INTERRUPT");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    int socket = crear_conexion(ipCPU,puertoServidor);
    op_code codigo_operacion;
    int  __attribute__((unused)) tamaño_paquete;
    void *msg;
    char *input = "hola";
    t_paquete *paquete;
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;

    paquete = crear_paquete(MENSAJE);
        agregar_a_paquete(paquete, (void *)input, strlen(input) + 1);
        printf("\nMensaje enviado.\nEsperando respuesta...\n");
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
