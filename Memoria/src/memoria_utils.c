#include "memoria_utils.h"

void esperar_hilos(){
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void recibir_handshake(int socketFd, op_code codigo) {
    char *cliente = (codigo == INIT_KERNEL) ? "Kernel" : "CPU"; 
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

void aplicar_retardoMemoria(){
    sleep(configMemoria.retardoMemoria / 1000); //retardo está en milisegundos
}

void aplicar_retardoSwap(){
    sleep(configMemoria.retardoSwap / 1000);
}

uint32_t recibir_uint32t(int socket){
    uint32_t valor;
    void *msg = recibir(socket);
    valor = *((uint32_t *)msg);
    free(msg);
    return valor;
}