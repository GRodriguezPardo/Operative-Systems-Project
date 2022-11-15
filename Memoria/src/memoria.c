#include "memoria.h"

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
    inicializar_espacio_usuario();
    inicializar_espacio_tablas();
    inicializar_lista_pageFaults();
    inicializar_mapas();

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
    iniciar_servidor(ConfigMemoria.ip, ConfigMemoria.puerto, atender_conexion);
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
    ConfigMemoria.ip = config_get_string_value(config,"IP_MEMORIA");
    ConfigMemoria.puerto = config_get_string_value(config, "PUERTO_ESCUCHA");
    ConfigMemoria.tamanioMemoria = (uint32_t)config_get_int_value(config, "TAM_MEMORIA");
    ConfigMemoria.tamanioPagina = (uint32_t)config_get_int_value(config, "TAM_PAGINA");
    ConfigMemoria.paginasPorTabla = (uint32_t)config_get_int_value(config, "ENTRADAS_POR_TABLA");
    ConfigMemoria.retardoMemoria = (uint32_t)config_get_int_value(config, "RETARDO_MEMORIA");
    ConfigMemoria.algoritmoReemplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
    ConfigMemoria.marcosPorProceso = (uint32_t)config_get_int_value(config, "MARCOS_POR_PROCESO");
    ConfigMemoria.retardoSwap = (uint32_t)config_get_int_value(config, "RETARDO_SWAP");
    ConfigMemoria.pathSwap = config_get_string_value(config, "PATH_SWAP");
    ConfigMemoria.tamanioSwap = (uint32_t)config_get_int_value(config, "TAMANIO_SWAP");

    ConfigMemoria.cantidadMarcosMemoria = ConfigMemoria.tamanioMemoria / ConfigMemoria.tamanioPagina;
    ConfigMemoria.cantidadPaginasSwap = ConfigMemoria.tamanioSwap / ConfigMemoria.tamanioPagina;
}

void finalizar_memoria(){
    free(EspacioUsuario);
    swap_cerrar();
    destruir_espacio_tablas();
    destruir_lista_pageFaults();
    destruir_mapas();
    config_destroy(config);

    loggear_info(logger, "Módulo memoria finalizado.");
    log_destroy(logger);
    cerrarMutexes();
}

/* FUNCIONES PRIVADAS */

void inicializar_espacio_usuario(){
    EspacioUsuario = malloc(ConfigMemoria.tamanioMemoria); //espacio de usuario
    pthread_mutex_init(&mx_espacioUsuario, NULL);
    loggear_info(logger, "Espacio memoria inicializado.");
}

void inicializar_espacio_tablas(){
    EspacioTablasPag = list_create(); //estructura para las tablas de páginas
    pthread_mutex_init(&mx_espacioTablasPag, NULL);
    loggear_info(logger, "Espacio Kernel inicializado.");
}

void inicializar_lista_pageFaults(){
    ListaPageFaults = list_create();
    pthread_mutex_init(&mx_listaPageFaults, NULL);
    loggear_info(logger, "Lista Page Faults inicializada.");
}

void inicializar_mapas(){
    int tamBitmap_bytes = ConfigMemoria.cantidadMarcosMemoria / 8;
    if((ConfigMemoria.cantidadMarcosMemoria % 8) != 0)
        tamBitmap_bytes++; //si queda un resto, agrego un byte extra

    void* puntero_a_bits = malloc(tamBitmap_bytes);
    MapaFrames = bitarray_create_with_mode(puntero_a_bits, tamBitmap_bytes, LSB_FIRST);

    //--------

    tamBitmap_bytes = ConfigMemoria.cantidadPaginasSwap / 8;
    if((ConfigMemoria.cantidadPaginasSwap % 8) != 0)
        tamBitmap_bytes++; //si queda un resto, agrego un byte extra

    puntero_a_bits = malloc(tamBitmap_bytes);
    MapaSwap = bitarray_create_with_mode(puntero_a_bits, tamBitmap_bytes, LSB_FIRST);
}

void destruir_espacio_tablas(){
    list_destroy_and_destroy_elements(EspacioTablasPag, &pag_destruirTablaPaginas);
}

void destruir_lista_pageFaults(){
    list_destroy_and_destroy_elements(ListaPageFaults, &pag_destruirPagina);
}

void destruir_mapas(){
    bitarray_destroy(MapaFrames);
    bitarray_destroy(MapaSwap);
}

void cerrarMutexes(){
    pthread_mutex_destroy(&mx_espacioUsuario);
    pthread_mutex_destroy(&mx_espacioTablasPag);
    pthread_mutex_destroy(&mx_listaPageFaults);
    pthread_mutex_destroy(&mx_main);
    pthread_mutex_destroy(&mx_logger);
}