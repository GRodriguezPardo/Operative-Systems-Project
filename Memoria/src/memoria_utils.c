#include "memoria_utils.h"

void esperar_hilos(){
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void finalizar_memoria(){
    free(memoriaPrincipal);
    swap_cerrar();

    config_destroy(config);
    log_destroy(logger);
    
    pthread_mutex_destroy(&mx_main);
    pthread_mutex_destroy(&mx_logger);
}

void recibirHandshake(int socketFd, op_code codigo) {
    char *cliente = (codigo == KERNEL_ACK) ? "Kernel" : "CPU"; 
    printf("Se conectó %s.", cliente);
    //log_debug(logger, "Se conectó %s.", cliente);

    char *handshakeMsg = (char *)recibir(socketFd);
    printf("ACK Recibido: %s", handshakeMsg);
    //log_debug(logger, "ACK Recibido: %s", handshakeMsg);
    free(handshakeMsg);
}

void loggear_info(t_log *log, char *msg){
    pthread_mutex_lock(&mx_logger);
    log_info(log, msg);
    pthread_mutex_unlock(&mx_logger);
}

void loggear_error(t_log *log, char *msg){
    pthread_mutex_lock(&mx_logger);
    log_error(log, msg);
    pthread_mutex_unlock(&mx_logger);
}