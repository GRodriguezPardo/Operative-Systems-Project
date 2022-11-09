#include "cpu_routine.h"

static void responder_handshakeCPU(int socket);
uint32_t leer_memoria(uint32_t direccionFisica);
void escribir_memoria(uint32_t direccionFisica, uint32_t valor);
void devolver_cpu(int socket, op_code code, uint32_t valor);

void cpu_routine(int socketFd, int *returnStatus){
    uint32_t valorRespuesta = 0;
    op_code codRespuesta;
    uint32_t direccionFisica;

    recibir_handshake(socketFd, INIT_CPU);
    responder_handshakeCPU(socketFd);

    while(1){
        op_code codPeticion = recibir_operacion(socketFd);
        codRespuesta = codPeticion;
        int __attribute__((unused)) tamanio = largo_paquete(socketFd);

        aplicar_retardo(configMemoria.retardoMemoria);
        switch (codPeticion)
        {
        case MOV_IN:
            direccionFisica = recibir_uint32t(socketFd); //recibo la direccion
            valorRespuesta = leer_memoria(direccionFisica);
            break;
        case MOV_OUT:
            direccionFisica = recibir_uint32t(socketFd); //recibo la direccion
            uint32_t valorAEscribir = recibir_uint32t(socketFd); //recibo el valor a escribir
            escribir_memoria(direccionFisica, valorAEscribir);
            break;
        case MMU_MARCO:
            uint32_t idTabla = recibir_uint32t(socketFd);
            uint32_t numPagina = recibir_uint32t(socketFd);
            uint32_t marco;

            if (pag_obtenerMarcoPagina(idTabla, numPagina, &marco) == -1)
                codRespuesta = PAGE_FAULT;
            else 
                valorRespuesta = marco;

            break;
        default:
            break;
        }

        devolver_cpu(socketFd, codRespuesta, valorRespuesta);
    }

    pthread_mutex_unlock(&mx_main);
    pthread_exit(returnStatus);
}

static void responder_handshakeCPU(int socket){
    //mandar entradasXtablaPaginas y tamanioPagina
    t_paquete *pack = crear_paquete(INIT_CPU);
    agregar_a_paquete(pack, (void *)&(configMemoria.entradasPorTabla), sizeof(configMemoria.entradasPorTabla));
    agregar_a_paquete(pack, (void *)&(configMemoria.tamanioPagina), sizeof(configMemoria.tamanioPagina));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);

    char *msg = string_from_format("CPU Thread :: entradasPorTabla: %d // tamanioPagina: %d enviados.", configMemoria.entradasPorTabla, configMemoria.tamanioPagina);
    loggear_info(logger, msg);
    free(msg);
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