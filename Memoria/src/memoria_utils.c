#include "memoria_utils.h"

static void responder_handshake(int, op_code);

void memoria_iniciar() {
    config = config_create("../memoria.config");
    logger = log_create("../memoria.log", "Memoria - Main", 0, LOG_LEVEL_INFO);
    pthread_mutex_init(&mx_logger, NULL);

    /// Inicializacion espacio de memoria ///
    int tamanioMemoria = config_get_int_value(config, "TAM_MEMORIA");
    memoriaPrincipal = malloc(tamanioMemoria);
    pthread_mutex_init(&mx_memoria, NULL);

    /// Inicializacion tablas segmentos / paginas
    int _cantEntradasXtabla = config_get_int_value(config, "ENTRADAS_POR_TABLA");
    int _tamanioPagina = config_get_int_value(config, "TAM_PAGINA");
    tamanioMaxSegmento = _cantEntradasXtabla * _tamanioPagina;
    cantMaxMarcosXProceso = config_get_int_value(config, "MARCOS_POR_PROCESO");

    /// Inicializacion SWAP
    swap_inicializar();
}

void esperarHilos()
{
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void memoria_finalizar(){
    free(memoriaPrincipal);
    swap_cerrar();

    config_destroy(config);
    log_destroy(logger);
    
    pthread_mutex_destroy(&mx_main);
    pthread_mutex_destroy(&mx_logger);
}

void resolverHandshake(int socketFd, op_code codigo) {
    char *cliente = (codigo == KERNEL_ACK) ? "Kernel" : "CPU"; 
    printf("Se conectó %s.", cliente);
    //log_debug(logger, "Se conectó %s.", cliente);

    char *handshakeMsg = (char *)recibir(socketFd);
    printf("ACK Recibido: %s", handshakeMsg);
    //log_debug(logger, "ACK Recibido: %s", handshakeMsg);
    free(handshakeMsg);

    responder_handshake(socketFd, codigo);
}

static void responder_handshake(int socketFd, op_code codigo){
	char* response = "OK.";
	int largo = strlen(response);
	t_paquete *pack = crear_paquete(codigo);
    agregar_a_paquete(pack, (void *) response, largo);
    enviar_paquete(pack, socketFd);
}