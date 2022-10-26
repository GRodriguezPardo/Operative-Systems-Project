#include "memoria.h"
#include <string.h>

int main(int argc, char** argv){
    inicializarMemoria();

    ////////////// CREANDO SERVER //////////////
    puts("Creando server de memoria.");
    pthread_t threadSv;
    if (pthread_create(&threadSv, NULL, &crearServidorMemoria, (void *) config) < 0) {
        perror("Thread de memoria falló.");
        return EXIT_FAILURE;
    }

    esperarHilos();
    printf("Finalizando módulo memoria...");
    finalizarMemoria();
    return EXIT_SUCCESS;
}

void *crearServidorMemoria(void * config){
    int *return_status = (int*)calloc(1, sizeof(int));
    char* ipMemoria = config_get_string_value(config,"IP_MEMORIA");
    char* puertoEscucha = config_get_string_value(config, "PUERTO_ESCUCHA");
    iniciar_servidor(ipMemoria, puertoEscucha, atenderConexion);
    pthread_exit(return_status);
}

void *atenderConexion(void * socketFd){
    int *return_status = (int*)malloc(sizeof(int));
    int socket = *(int *) socketFd;
    *return_status = 0;
    op_code cod_operacion = recibir_operacion(socket);

    resolverHandshake(socket, cod_operacion);
    switch (cod_operacion)
    {
    case CPU_ACK:
        cpu_routine(socket, return_status);
        break;
    case KERNEL_ACK:
        kernel_routine(socket, return_status);
        break;
    default:
        break;
    }

    pthread_mutex_unlock(&mx_main);
    pthread_exit(return_status);
}