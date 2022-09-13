#include<stdio.h>
#include "../../Libreria/tcp_client.c"
#include "commons/log.h"
#include "../../Libreria/tcp_serializacion.c"
#include "commons/config.h"
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>

int main(int argc, char** argv[]){
    //archivo config
    
    t_config* config = config_create(argv[0]);
    //file* archivo= fopen(argv[1],) abrir archivo en argv 1 o 2
    //parseo de lineas de comando, 
    
    int socket = crear_conexion("127.0.0.1","8000");
    //enviar lineas parseadas y serializadas
    
    recibir_operacion(socket);//se queda esperando mensajes

    return 0;
}




