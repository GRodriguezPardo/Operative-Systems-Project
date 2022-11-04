#include "cpu_routine.h"

static void responder_handshakeCPU(int socket);
uint32_t leer_memoria(uint32_t direccionFisica);
void escribir_memoria(uint32_t direccionFisica, uint32_t valor);
void devolver_cpu(int socket, op_code code, uint32_t valor);

void cpu_routine(int socketFd, int *returnStatus){
    uint32_t direccionFisica;
    uint32_t valor;

    recibir_handshake(socketFd, INIT_CPU);
    responder_handshakeCPU(socketFd);

    while(1){
        op_code op = recibir_operacion(socketFd);
        switch (op)
        {
        case MOV_IN:
            direccionFisica = recibir_uint32t(socketFd); //recibo la direccion
            valor = leer_memoria(direccionFisica);
            devolver_cpu(socketFd, op, valor);
            break;
        case MOV_OUT:
            direccionFisica = recibir_uint32t(socketFd); //recibo la direccion
            valor = recibir_uint32t(socketFd); //recibo el valor a escribir
            escribir_memoria(direccionFisica, valor);
            devolver_cpu(socketFd, op, 0);
            break;
        case MMU_MARCO:
            uint32_t idTabla = recibir_uint32t(socketFd);
            uint32_t numPagina = recibir_uint32t(socketFd);
            uint32_t marco = pag_obtenerMarcoPagina(idTabla, numPagina);

            if (marco == -1)
                devolver_cpu(socketFd, PAGE_FAULT, NULL);
            else
                devolver_cpu(socketFd, op, marco);
                           
            break;
        default:
            break;
        }
    }
}

static void responder_handshakeCPU(int socket){ 
    //mandar entradasXtablaPaginas y tamanioPagina
    t_paquete *pack = crear_paquete(INIT_CPU);
    agregar_a_paquete(pack, (void *)&(configMemoria.entradasPorTabla), sizeof(configMemoria.entradasPorTabla));
    agregar_a_paquete(pack, (void *)&(configMemoria.tamanioPagina), sizeof(configMemoria.tamanioPagina));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
}

uint32_t leer_memoria(uint32_t offset){
    uint32_t valor;
    pthread_mutex_lock(&mx_memoriaPrincipal);
    valor = *(uint32_t *)(memoriaPrincipal + offset);
    pthread_mutex_unlock(&mx_memoriaPrincipal);
    return valor;
}

void escribir_memoria(uint32_t offset, uint32_t valor){
    pthread_mutex_lock(&mx_memoriaPrincipal);
    memcpy(memoriaPrincipal + offset, &valor, sizeof(uint32_t));
    pthread_mutex_unlock(&mx_memoriaPrincipal);
}

void devolver_cpu(int socket, op_code code, uint32_t valor){
    t_paquete *pack = crear_paquete(code);
    agregar_a_paquete(pack, (void *)&valor, sizeof(valor));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
}

    /*
    while (true)

    - recibir operacion
        MOV_IN (lectura)
            recibo: direccion fisica
            - leer de memoria
            - devolver valor
        MOV_OUT (escritura)
            recibo: dir fisica + valor
            - escribir en memoria
            - devolver OK
        MMU_FETCH
            recibo: id_tabla + num_pagina
            - pag_obtenerMarcoPagina();
            - devolver num_marco || PAGE_FAULT
    */
    