#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "consola_routine.h"


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
            case -1:
                perror("Recibi OP_CODE = 0.");
                *return_status = 1;
                pthread_exit(return_status);
                break;
            default:
                perror("Recibí una operacion inesperada. Terminando programa.\n");
                printf("%d",codigo_operacion);
                *return_status = 1;
                pthread_exit(return_status);
                break;          
        }
    }
    pthread_exit(return_status);
}
