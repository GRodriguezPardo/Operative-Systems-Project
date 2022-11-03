#include "cpu_routine.h"

static void responderHandshakeCPU(int);

void cpu_routine(int socketFd, int *returnStatus){
    recibirHandshake(socketFd, CPU_ACK);
    responderHandshakeCPU(socketFd);
}

static void responderHandshakeCPU(int socketFd){ 
    //mandar entradasXtablaPaginas y tamanioPagina
    t_paquete *pack = crear_paquete(CPU_ACK);
    agregar_a_paquete(pack, (void *)&(configMemoria.entradas_x_tabla), sizeof(configMemoria.entradas_x_tabla));
    agregar_a_paquete(pack, (void *)&(configMemoria.tamanio_pagina), sizeof(configMemoria.tamanio_pagina));
    enviar_paquete(pack, socketFd);
    eliminar_paquete(pack);
}