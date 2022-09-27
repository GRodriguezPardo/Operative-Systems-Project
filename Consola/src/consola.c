#include <commons/config.h>
#include <commons/log.h>
#include <readline/readline.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>

int main(int argc, char **argv)
{
    ////////////// CONFIG //////////////
    t_config *config = config_create(argv[1]);

    char *PUERTO_KERNEL = config_get_string_value(config, "PUERTO_KERNEL");
    char *IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    char **segmentos = config_get_array_value(config, "SEGMENTOS");

    ////////////// TCP CLIENT //////////////
    int socket = crear_conexion(IP_KERNEL, PUERTO_KERNEL);

    ////////////// CREACION PAQUETE //////////////
    t_paquete *paquete_inicial = crear_paquete(NUEVO_PROCESO);

    ////////////// EMPAQUETADO SEGMENTOS //////////////
    uint32_t *tamanio_segmento = (uint32_t *)calloc(1, sizeof(uint32_t));
    for (size_t i = 0; i < 4; i++)
    {
        *tamanio_segmento = (uint32_t)strtoul(segmentos[i], NULL, 10);
        agregar_a_paquete(paquete_inicial, (void *)tamanio_segmento, sizeof(uint32_t));
    }

    ////////////// ARCHIVO INSTRUCCIONES Y EMPAQUETADO //////////////
    FILE *archivo = fopen(argv[2], "r"); // abrir archivo en argv 2
    char *instruccion;
    size_t len = 254;
    ssize_t nread;

    while ((nread = getline(&instruccion, &len, archivo)) != -1)
    {
        instruccion[strcspn(instruccion, "\n")] = 0;
        printf("%s\n", instruccion);
        agregar_a_paquete(paquete_inicial, (void *)instruccion, strlen(instruccion) + 1);
        free(instruccion);
        instruccion = NULL;
    };

    fclose(archivo);

    ////////////// ENVIO DE PAQUETE //////////////
    enviar_paquete(paquete_inicial, socket);
    eliminar_paquete(paquete_inicial);

    ////////////// ESPERA DE I/O //////////////
    op_code codigo_operacion;
    int tamaño_paquete;
    char *input = NULL, *output = NULL;
    t_paquete *paquete = NULL;

    codigo_operacion = recibir_operacion(socket);
    tamaño_paquete = largo_paquete(socket);

    while (codigo_operacion != EXIT)
    {
        switch (codigo_operacion)
        {
        case CONSOLE_INPUT:
            input = readline("> ");

            paquete = crear_paquete(CONSOLE_INPUT_RESPUESTA);
            agregar_a_paquete(paquete, (void *)input, strlen(input) + 1);
            enviar_paquete(paquete, socket);
            eliminar_paquete(paquete);
            paquete = NULL;

            free(input);
            input = NULL;

            break;
        case CONSOLE_OUTPUT:
            output = (char *)recibir(socket);
            printf("%s\n", output);
            free(output);
            output = NULL;

            paquete = crear_paquete(CONSOLE_OUTPUT_RESPUESTA);
            enviar_paquete(paquete, socket);
            eliminar_paquete(paquete);
            paquete = NULL;

            break;
        case -1:
            perror("El servidor 'KERNEL' se ha caido.");
        default:
            perror("Ha ocurrido un error. Finalizando programa.\n");
            liberar_conexion(socket);
            return EXIT_FAILURE;
            break;
        }

        codigo_operacion = recibir_operacion(socket);
        tamaño_paquete = largo_paquete(socket);
    }

    liberar_conexion(socket);

    return EXIT_SUCCESS;

    ////////////// EJEMPLO INICIAL //////////////
    /* op_code codigo_operacion;
    int tamaño_paquete;
    void *msg;
    char *input;
    char *linea;
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
    */
}
