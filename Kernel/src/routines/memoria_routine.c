#include "memoria_routine.h"

void *memoria_routine(void *config_aux)
{
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    t_config *config = (t_config *)config_aux;

    int socketMemoria = conectarAMemoria(config);
    iniciarHandshake(socketMemoria);

    //TODO resto de la rutina

    pthread_exit(return_status);
    return 0;
}

static int conectarAMemoria(t_config *config){
    int socketFd;
    char *ip = config_get_string_value(config, "IP_MEMORIA");
    char *puerto = config_get_string_value(config, "PUERTO_MEMORIA");
    socketFd = crear_conexion(ip, puerto);
    return socketFd;
}

static void iniciarHandshake(int socket){

    t_paquete *pack = crear_paquete(KERNEL_ACK);
    char *msg = "";
    agregar_a_paquete(pack, msg, string_length(msg) * sizeof(char));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
    //recibir respuesta
    op_code code = recibir_operacion(socket);
    /*if (code != KERNEL_ACK) 
        return 1;
    */
    char* respuesta = (char *) recibir(socket);
    if (strcmp(respuesta, "OK."))
        perror("El handshake fue incorrecto.");
}