#include "memoria_utils.h"

static void responder_handshake(int, op_code);
static void loggearLevel(t_log*, const char*, t_log_level);

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
	char* response = "Handshake recibido.";
	int largo = strlen(response);
	t_paquete *pack = crear_paquete(codigo);
    agregar_a_paquete(pack, (void *) response, largo);
    enviar_paquete(pack, socketFd);
}

void inicializarMemoria() {
    config = config_create("../memoria.config");
    logger = log_create("../memoria.log", "Memoria - Main", 0, LOG_LEVEL_INFO);
    pthread_mutex_init(&mutexLogger, NULL);
}

void esperarHilos()
{
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void finalizarMemoria(){
    config_destroy(config);
    log_destroy(logger);
    pthread_mutex_destroy(&mx_main);
    pthread_mutex_destroy(&mutexLogger);
}