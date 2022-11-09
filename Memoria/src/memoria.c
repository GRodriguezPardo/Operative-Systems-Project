#include "memoria.h"
#include <string.h>

static void liberar_espacioKernel();

int main(int argc, char** argv){
    logger = log_create("../memoria.log", "Memoria - Main", 1, LOG_LEVEL_INFO);
    pthread_mutex_init(&mx_logger, NULL);

    loggear_info(logger, "Iniciando módulo memoria...");

    // Iniciando config y logs
    config = config_create("../memoria.config");
    if (config == NULL)
    {
        loggear_error(logger, "Archivo de configuración no encontrado.");
        exit(EXIT_FAILURE);
    }

    cargar_configuracion_memoria();
    loggear_info(logger, "Configuración memoria cargada.");

    // Inicializando espacio de datos y de Kernel
    memoriaPrincipal = malloc(configMemoria.tamanioMemoria); //espacio de usuario
    pthread_mutex_init(&mx_memoriaPrincipal, NULL);
    loggear_info(logger, "Espacio memoria inicializado.");
    espacioKernel = list_create(); //estructura para las tablas de páginas
    pthread_mutex_init(&mx_espacioKernel, NULL);
    loggear_info(logger, "Espacio Kernel inicializado.");

    swap_inicializar();
    loggear_info(logger, "Espacio SWAP inicializado.");

    // Creando Server
    loggear_info(logger, "Iniciando server...");
    pthread_t threadSv;
    if (pthread_create(&threadSv, NULL, &levantar_server_memoria, (void *) config) < 0) {
        loggear_error(logger, "Error al iniciar servidor.");
        exit(EXIT_FAILURE);
    }
    loggear_info(logger, "Servidor OK.");

    esperar_hilos();
    loggear_info(logger, "Finalizando módulo memoria...");
    finalizar_memoria();
    return EXIT_SUCCESS;
}

void *levantar_server_memoria(void * config){
    int *return_status = (int*)calloc(1, sizeof(int));
    iniciar_servidor(configMemoria.ip, configMemoria.puerto, atender_conexion);
    pthread_exit(return_status);
}

void *atender_conexion(void *arg){
    int *return_status = (int*)malloc(sizeof(int));
    int socket = *(int *)arg;
    *return_status = 0;
    op_code cod_operacion = recibir_operacion(socket);
    switch (cod_operacion)
    {
    case INIT_CPU:
        cpu_routine(socket, return_status);
        break;
    case INIT_KERNEL:
        kernel_routine(socket, return_status);
        break;
    default:
        break;
    }
}

void cargar_configuracion_memoria(){
    configMemoria.ip = config_get_string_value(config,"IP_MEMORIA");
    configMemoria.puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    configMemoria.tamanioMemoria = (uint32_t)config_get_int_value(config, "TAM_MEMORIA");
    configMemoria.tamanioPagina = (uint32_t)config_get_int_value(config, "TAM_PAGINA");
    configMemoria.entradasPorTabla = (uint32_t)config_get_int_value(config, "ENTRADAS_POR_TABLA");
    configMemoria.retardoMemoria = (uint32_t)config_get_int_value(config, "RETARDO_MEMORIA");
    configMemoria.algoritmoReemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    configMemoria.marcosPorProceso = (uint32_t)config_get_int_value(config, "MARCOS_POR_PROCESO");
    configMemoria.retardoSwap = (uint32_t)config_get_int_value(config, "RETARDO_SWAP");
    configMemoria.pathSwap = config_get_string_value(config, "PATH_SWAP");
    configMemoria.tamanioSwap = (uint32_t)config_get_int_value(config, "TAMANIO_SWAP");
}

void finalizar_memoria(){
    free(memoriaPrincipal);
    liberar_espacioKernel();
    swap_cerrar();

    config_destroy(config);  
    pthread_mutex_destroy(&mx_main);

    loggear_info(logger, "Módulo memoria finalizado.");
    pthread_mutex_destroy(&mx_logger);
    log_destroy(logger);
}

/// @brief Libera el espacio de tablas de páginas.
static void liberar_espacioKernel(){
    list_destroy_and_destroy_elements(espacioKernel, &pag_destruirTablaPaginas);
    pthread_mutex_destroy(&mx_espacioKernel);
}