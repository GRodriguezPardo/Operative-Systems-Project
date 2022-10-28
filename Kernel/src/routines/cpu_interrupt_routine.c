#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_client.h>
#include <thesenate/tcp_serializacion.h>
#include "cpu_interrupt_routine.h"
#include "../globals.h"

void *cpu_interrupt_routine(void *config)
{
    int socket;
    {
        char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_CPU_INTERRUPT");
        char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
        socket = crear_conexion(ipCPU,puertoServidor);
    }
    
    op_code codigo_operacion;
    int  __attribute__((unused)) tamaño_paquete;
    t_paquete *paquete;
    
    while(1)
    {
        clock_routine();

        paquete = crear_paquete(DESALOJO_PROCESO);
        enviar_paquete(paquete, socket);
        eliminar_paquete(paquete);

        printf("\nMensaje enviado.\nEsperando respuesta...\n");

        codigo_operacion = recibir_operacion(socket);
        tamaño_paquete = largo_paquete(socket);

        switch (codigo_operacion)
        {
            case RESPUESTA:
                continue;
                break;
            default:
                perror("Recibí una operacion inesperada. Terminando programa.");
                liberar_conexion(socket);
                exit(EXIT_FAILURE);
                break;
        }
    }
    
    printf("Terminando programa.\n");
    liberar_conexion(socket);

    exit(EXIT_SUCCESS);
    return NULL;
}
