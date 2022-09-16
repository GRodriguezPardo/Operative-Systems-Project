#include <commons/config.h>
#include <commons/log.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>


struct  instrucciones{
    char* instruccion;
    int segmentos[];
};

int main(int argc, char **argv)
{
    // archivo config
    printf("hola");
    t_config *config = config_create("../consola.config");
    /*FILE* archivo= fopen("../instrucciones.txt","r"); //abrir archivo en argv 2
    printf("abrio los archivos");
    if(archivo == NULL){
        return 0;
    }*/
    // parseo de lineas de comando,
    char* PUERTO_KERNEL = config_get_string_value(config, "PUERTO_KERNEL");
    char* IP_KERNEL = config_get_string_value(config,"IP_KERNEL");

    int socket = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    // enviar lineas parseadas y serializadas

    op_code codigo_operacion;
    int tamaño_paquete;
    void *msg;
    char *input;
    char* linea;
    t_paquete *paquete;

    //while(fscanf(archivo,"%s\n",linea) != EOF){
      //  printf(linea);
    //}

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
