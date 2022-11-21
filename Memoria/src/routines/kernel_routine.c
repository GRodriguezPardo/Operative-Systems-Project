#include "kernel_routine.h"

static void responder_OK(int socketFd, op_code code);

void kernel_routine(int socketKernel, int* returnStatus) {
    recibir_handshake(socketKernel, INIT_KERNEL);
    responder_OK(socketKernel, INIT_KERNEL);
    t_segmento_pcb* segmentos;
    void * msg;

    while (1) {
        op_code operacion = recibir_operacion(socketKernel);
        int tamanioPaquete __attribute_maybe_unused__ = largo_paquete(socketKernel);
        uint32_t idProceso;

        switch (operacion)
        {
        case NUEVO_PROCESO:
            idProceso = recibir_uint32t(socketKernel);
            uint32_t cantidadSegmentos = recibir_uint32t(socketKernel);
            msg = recibir(socketKernel);
            segmentos = (t_segmento_pcb*)msg;
            free(msg);
            msg = NULL;

            //NOTA: chequear si es necesario recibir los tamaños (si influyen o no al reservar espacio en swap).
            //si siempre se reserva el maximo de un segmento, solo recibo la cantidad

            t_paquete *packRespuesta = crear_paquete(NUEVO_PROCESO);

            for (uint32_t i = 0; i < cantidadSegmentos; i++)
            {
                uint32_t idTabla = pag_crearTablaPaginas(idProceso,segmentos->tamanio);                
                agregar_a_paquete(packRespuesta, (void *)&idTabla, sizeof(uint32_t));
            }

            enviar_paquete(packRespuesta, socketKernel);
            eliminar_paquete(packRespuesta);
            
            break;
        case EXIT_PROCESO:
            idProceso = recibir_uint32t(socketKernel);
            puts("LIBERANDO SWAP..");//liberar el swap
            puts("ACTUALIZAND FRAMES..");//actualizar tabla de frames

            responder_OK(socketKernel, EXIT_PROCESO);
        case SWAP:
            idProceso = recibir_uint32t(socketKernel);
            //liberar el swap
            //actualizar tabla de frames
            responder_OK(socketKernel, SWAP);
        default:
            return;
            break;
        }
    }
    
    /*
    PENDIENTES

        NUEVO
            - cargar proceso en swap
            - manejar tabla (bitmap) de frames libres
        EXIT
            recibo: id_proceso
            - libero swap
            - si hay bitmap de marcos, actualizar
            - devolver OK
        SWAP
            recibo: id_proceso + id_tabla + num_pagina
            - hacer swap de pagina
            - devolver OK
    */
    *returnStatus = EXIT_SUCCESS;
    pthread_mutex_unlock(&mx_main);
    pthread_exit(returnStatus);
}

static void responder_OK(int socket, op_code code){
    t_paquete *pack = crear_paquete(code);
    char *msg = "OK";
    agregar_a_paquete(pack, (void *)msg, string_length(msg) * sizeof(char) + 1);
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);

    loggear_info(logger, "Kernel Thread :: OK respondido");
}