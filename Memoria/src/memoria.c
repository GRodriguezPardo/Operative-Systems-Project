#include "memoria.h"

void remove_if_exists(const char *fname);

int main(int argc, char** argv){
    remove_if_exists("../memoriaMain.log");
    loggerMain = log_create("../memoriaMain.log", "Memoria - Main", false, LOG_LEVEL_INFO);
    pthread_mutex_init(&mx_loggerMain, NULL);

    remove_if_exists("../memoriaAuxiliares.log");
    loggerAux = log_create("../memoriaAuxiliares.log", "Memoria - Auxiliares", false, LOG_LEVEL_INFO);
    pthread_mutex_init(&mx_loggerAux, NULL);

    loggear_info(loggerAux, "Iniciando módulo memoria...", false);

    // Iniciando config y logs
    config = config_create("../memoria.config");
    if (config == NULL)
    {
        loggear_error(loggerAux, "Archivo de configuración no encontrado.", false);
        exit(EXIT_FAILURE);
    }

    cargar_configuracion_memoria();
    loggear_info(loggerAux, "Configuración memoria cargada.", false);

    // Inicializando espacio de datos y de Kernel
    inicializar_espacio_usuario();
    inicializar_espacio_tablas();
    inicializar_tabla_frames();
    inicializar_mapas();

    swap_inicializar();
    loggear_info(loggerAux, "Espacio SWAP inicializado.", false);

    // Creando Server
    loggear_info(loggerAux, "Iniciando server...", false);
    pthread_t threadSv;
    if (pthread_create(&threadSv, NULL, &levantar_server_memoria, (void *) config) < 0) {
        loggear_error(loggerAux, "Error al iniciar servidor.", false);
        exit(EXIT_FAILURE);
    }
    loggear_info(loggerAux, "Servidor OK.", false);

    esperar_hilos();
    loggear_info(loggerAux, "Finalizando módulo memoria...", false);
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
    return NULL;
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
    destruir_tabla_frames();
    destruir_mapas();
    config_destroy(config);

    loggear_info(loggerAux, "Módulo memoria finalizado.", false);
    log_destroy(loggerAux);
    log_destroy(loggerMain);
    cerrarMutexes();
}

/* FUNCIONES PRIVADAS */

void inicializar_espacio_usuario(){
    EspacioUsuario = malloc(ConfigMemoria.tamanioMemoria); //espacio de usuario
    pthread_mutex_init(&mx_espacioUsuario, NULL);
    loggear_info(loggerAux, "Espacio memoria inicializado.", false);
}

void inicializar_espacio_tablas(){
    EspacioKernel = dictionary_create();
    pthread_mutex_init(&mx_espacioKernel, NULL);
    loggear_info(loggerAux, "Espacio Kernel inicializado.", false);
}

void inicializar_tabla_frames(){
    TablaFrames = dictionary_create();
    pthread_mutex_init(&mx_tablaFrames, NULL);
    loggear_info(loggerAux, "Tabla de Frames inicializada.", false);
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
    dictionary_destroy_and_destroy_elements(EspacioKernel, &pag_destruirTablaPaginas);
}

void destruir_tabla_frames(){
    dictionary_destroy_and_destroy_elements(TablaFrames, &free);
}

void destruir_mapas(){
    bitarray_destroy(MapaFrames);
    bitarray_destroy(MapaSwap);
}

void cerrarMutexes(){
    pthread_mutex_destroy(&mx_espacioUsuario);
    pthread_mutex_destroy(&mx_espacioKernel);
    pthread_mutex_destroy(&mx_tablaFrames);
    pthread_mutex_destroy(&mx_main);
    pthread_mutex_destroy(&mx_loggerMain);
    pthread_mutex_destroy(&mx_loggerAux);
}

void remove_if_exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        remove(fname);
    }
}
