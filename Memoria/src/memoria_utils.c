#include "memoria_utils.h"

void esperar_hilos(){
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void recibir_handshake(int socketFd, op_code codigo) {
    char *msg = string_from_format("Se conectó %s.", (codigo == INIT_KERNEL) ? "Kernel" : "CPU");
    loggear_info(logger, msg);
    free(msg);

    int __attribute__((unused)) tamanio = largo_paquete(socketFd);
    void *handshakeMsg = recibir(socketFd);
    msg = string_from_format("ACK Recibido: %d", *(int *)handshakeMsg);
    loggear_info(logger, msg);
    free(handshakeMsg);
    free(msg);
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

void aplicar_retardo(uint32_t tiempo_ms){
    sleep(tiempo_ms / 1000); //tiempo_ms está en milisegundos
}

uint32_t recibir_uint32t(int socket){
    uint32_t valor;
    void *buffer = recibir(socket);
    valor = *((uint32_t *)buffer);
    free(buffer);
    return valor;
}