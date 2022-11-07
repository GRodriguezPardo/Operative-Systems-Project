#include <commons/config.h>
#include <commons/log.h>
#include <readline/readline.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>

int main(int argc, char **argv)
{
    ////////////// CONFIG //////////////
    t_config *config = config_create(argv[1]);

    char *PUERTO_KERNEL = config_get_string_value(config, "PUERTO_KERNEL");
    char *IP_KERNEL = config_get_string_value(config, "IP_KERNEL");
    char **segmentos = config_get_array_value(config, "SEGMENTOS");
    int retardo_pantalla = config_get_int_value(config, "TIEMPO_PANTALLA");

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
        free(segmentos[i]);
    }
    free(segmentos);
    free(tamanio_segmento);

    ////////////// ARCHIVO INSTRUCCIONES Y EMPAQUETADO //////////////
    FILE *archivo = fopen(argv[2], "r"); // abrir archivo en argv 2
    char *instruccion = NULL;
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
    free(instruccion);

    fclose(archivo);

    ////////////// ENVIO DE PAQUETE //////////////
    enviar_paquete(paquete_inicial, socket);
    eliminar_paquete(paquete_inicial);

    ////////////// ESPERA DE I/O //////////////
    op_code codigo_operacion;
    int __attribute__((unused)) tamaño_paquete;
    char *auxInput = NULL;
    uint32_t *input = (uint32_t*)malloc(sizeof(uint32_t)), *output = NULL;
    t_paquete *paquete = NULL;

    codigo_operacion = recibir_operacion(socket);
    tamaño_paquete = largo_paquete(socket);

    while (codigo_operacion != EXIT)
    {
        switch (codigo_operacion)
        {
        case CONSOLE_INPUT:
            auxInput = readline("> ");
            *input = atoi(auxInput);

            paquete = crear_paquete(CONSOLE_INPUT_RESPUESTA);
            agregar_a_paquete(paquete, (void *)input, sizeof(uint32_t));
            enviar_paquete(paquete, socket);
            eliminar_paquete(paquete);
            paquete = NULL;

            free(auxInput);
            auxInput = NULL;
            break;
        case CONSOLE_OUTPUT:
            output = (uint32_t*)recibir(socket);
            printf("%u\n", *output);
            free(output);
            output = NULL;

            usleep(retardo_pantalla * 1000);

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

    free(input);
    config_destroy(config);

    liberar_conexion(socket);

    return EXIT_SUCCESS;
}
