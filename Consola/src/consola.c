#include <commons/config.h>
#include <commons/log.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>

int main(int argc, char **argv)
{
    // archivo config

    t_config *config = config_create(argv[1]);
    // file* archivo= fopen(argv[2],) abrir archivo en argv 2
    // parseo de lineas de comando,

    int socket = crear_conexion("127.0.0.1", "8000");
    // enviar lineas parseadas y serializadas

    op_code codigo_operacion;
    int tamaño_paquete;
    void *msg;
    char *input;
    t_paquete *paquete;

    input = readline("> ");
    while (strcmp(input, ""))
    {
        paquete = crear_paquete(MENSAJE);
        agregar_a_paquete(paquete, (void *)input, strlen(input) + 1);
        enviar_paquete(paquete, socket);
        eliminar_paquete(paquete);

        free(input);

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
                return 1;
                break;
        }

        input = readline("> ");
    }
    free(input);
    printf("Terminando programa.\n");
    liberar_conexion(socket);

    // recibir_operacion(socket); // se queda esperando mensajes

    return 0;
}
