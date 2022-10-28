#include <commons/config.h>
#include <commons/log.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_client.h>

void *memoria_routine(void *config_aux)
{
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    t_config *config = (t_config *)config_aux;

    int socketMemoria = conectarAMemoria(config);
    iniciarHandshake(socketMemoria);

    pthread_exit(return_status);
    return 0;
}

int conectarAMemoria(t_config *config){
    int socketFd;
    char *ip = config_get_string_value(config, "IP_MEMORIA");
    char *puerto = config_get_string_value(config, "PUERTO_MEMORIA");
    socketFd = crear_conexion(ip, puerto);
    return socketFd;
}

void iniciarHandshake(int socket){

    t_paquete *pack = crear_paquete(CPU_ACK);
    char *msg = "";
    agregar_a_paquete(pack, msg, string_length(msg) * sizeof(char));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
    //recibir respuesta
    op_code code = recibir_operacion(socket);

    char* respuesta = (char *) recibir(socket);
    if (strcmp(respuesta, "OK."))
        perror("El handshake fue incorrecto.");
}