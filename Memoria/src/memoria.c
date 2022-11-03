#include "memoria.h"
#include <string.h>

int main(int argc, char** argv){
    puts("Iniciando módulo memoria...");

    crear_config();
    crear_logger();    
    cargar_configuracion_memoria();

    // Inicializando espacio de datos
    memoriaPrincipal = malloc(configMemoria.tamanio_memoria);
    pthread_mutex_init(&mx_memoria, NULL);

    swap_inicializar();

    // Creando Server
    puts("Creando server de memoria.");
    pthread_t threadSv;
    if (pthread_create(&threadSv, NULL, &levantar_server_memoria, (void *) config) < 0) {
        perror("Thread de memoria falló.");
        return EXIT_FAILURE;
    }

    esperar_hilos();
    puts("Finalizando módulo memoria...");
    finalizar_memoria();
    return EXIT_SUCCESS;
}

void *levantar_server_memoria(void * config){
    int *return_status = (int*)calloc(1, sizeof(int));
    iniciar_servidor(configMemoria.ip, configMemoria.puerto, atender_conexion);
    pthread_exit(return_status);
}

void *atender_conexion(void *socketFd){
    int *return_status = (int*)malloc(sizeof(int));
    int socket = *(int *) socketFd;
    *return_status = 0;
    op_code cod_operacion = recibir_operacion(socket);
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


void crear_config(){
    config = config_create("../memoria.config");
    if (config == NULL)
    {
        perror("Archivo de configuración no encontrado.");
        exit(EXIT_FAILURE);
    }
}

void crear_logger(){
    logger = log_create("../memoria.log", "Memoria - Main", 0, LOG_LEVEL_INFO);
    pthread_mutex_init(&mx_logger, NULL);
}

void cargar_configuracion_memoria(){
    configMemoria.ip = config_get_string_value(config,"IP_MEMORIA");
    configMemoria.puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    configMemoria.tamanio_memoria = (uint32_t)config_get_int_value(config, "TAM_MEMORIA");
    configMemoria.tamanio_pagina = (uint32_t)config_get_int_value(config, "TAM_PAGINA");
    configMemoria.entradas_x_tabla = (uint32_t)config_get_int_value(config, "ENTRADAS_POR_TABLA");
    configMemoria.retardo_memoria = (uint32_t)config_get_int_value(config, "RETARDO_MEMORIA");
    configMemoria.algoritmo_reemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    configMemoria.marcos_x_proceso = (uint32_t)config_get_int_value(config, "MARCOS_POR_PROCESO");
    configMemoria.retardo_swap = (uint32_t)config_get_int_value(config, "RETARDO_SWAP");
    configMemoria.path_swap = config_get_string_value(config, "PATH_SWAP");
    configMemoria.tamanio_swap = (uint32_t)config_get_int_value(config, "TAMANIO_SWAP");
}