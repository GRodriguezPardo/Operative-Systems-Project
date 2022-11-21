#include "kernel_routine.h"

static void responder_OK(int socketFd, op_code code);

void kernel_routine(int socketKernel, int* returnStatus) {
    recibir_handshake(socketKernel, INIT_KERNEL);
    responder_OK(socketKernel, INIT_MEMORIA);

//    while (1) {
        op_code operacion = recibir_operacion(socketKernel);
        int __attribute_maybe_unused__ tamanioPaquete = largo_paquete(socketKernel);
        uint32_t pid = recibir_uint32t(socketKernel);
        uint32_t __attribute_maybe_unused__ cantidadSegmentos;

        switch (operacion)
        {
        case NUEVO_PROCESO:          
            cantidadSegmentos = recibir_uint32t(socketKernel);
            t_paquete *packRespuesta = crear_paquete(NUEVO_PROCESO);

            t_dataProceso *dataP = (t_dataProceso *)malloc(sizeof(t_dataProceso));
            dataP->tablasProceso = list_create();
            dataP->paginasPresentes = list_create();

            for (uint32_t n = 0; n < cantidadSegmentos; n++)
            {
                uint32_t _idTabla = pag_crearTablaPaginas(dataP->tablasProceso, n);
                agregar_a_paquete(packRespuesta, (void *)&_idTabla, sizeof(uint32_t));
                char *msg = string_from_format("Creacion de Tabla de Paginas -> PID: %d - Segmento: %d - TAMAÑO: %d páginas", pid, n, ConfigMemoria.paginasPorTabla);
                loggear_info(loggerMain, msg, true);
            }

            char *sPID = string_itoa(pid);
            pthread_mutex_lock(&mx_espacioTablasPag);
            dictionary_put(EspacioTablas, sPID, dataP);
            pthread_mutex_unlock(&mx_espacioTablasPag);
            free(sPID);

            enviar_paquete(packRespuesta, socketKernel);
            eliminar_paquete(packRespuesta);
            break;
        case EXIT_PROCESO:
            liberar_proceso(pid);
            responder_OK(socketKernel, EXIT_PROCESO);

            char *msg = string_from_format("Destrucción de Tabla de Paginas -> PID: %d", pid);
            loggear_info(loggerMain, msg, true);
            break;
        case SWAP:
            uint32_t idTabla = recibir_uint32t(socketKernel);
            uint32_t numPagina = recibir_uint32t(socketKernel);

            uint32_t numFrame = swap_resolver_pageFault(pid, idTabla, numPagina);
            crearEntradaTablaFrames(numFrame, pid, idTabla, numPagina);
            responder_OK(socketKernel, SWAP);
            break;
        default:
            *returnStatus = EXIT_FAILURE;
            pthread_mutex_unlock(&mx_main);
            pthread_exit(returnStatus);
            break;
        }
//    }
    
    *returnStatus = EXIT_SUCCESS;
    pthread_mutex_unlock(&mx_main);
    pthread_exit(returnStatus);
}

void liberar_proceso(uint32_t pid){
    t_dataProceso *dataP;

    char *sPID = string_itoa(pid);
    pthread_mutex_lock(&mx_espacioTablasPag);
    dataP = (t_dataProceso *)dictionary_remove(EspacioTablas, sPID);
    pthread_mutex_unlock(&mx_espacioTablasPag);
    free(sPID);

    list_destroy(dataP->paginasPresentes);
    list_destroy_and_destroy_elements(dataP->tablasProceso, pag_destruirTablaPaginas);
    free(dataP);
}

void crearEntradaTablaFrames(uint32_t numFrame, uint32_t pid, uint32_t idTabla, uint32_t numPagina){
    t_infoFrame *info = (t_infoFrame *)malloc(sizeof(t_infoFrame));
    info->pid = pid;
    info->idTabla = idTabla;
    info->numPagina = numPagina;
    
    char *sNumFrame = string_itoa(numFrame);
    dictionary_put(TablaFrames, sNumFrame, info);
    free(sNumFrame);
}

void borrarEntradaTablaFrames(uint32_t numFrame){
    char *sNumFrame = string_itoa(numFrame);
    if(dictionary_has_key(TablaFrames, sNumFrame)){
        dictionary_remove_and_destroy(TablaFrames, sNumFrame, &free);
    }
    free(sNumFrame);
}


static void responder_OK(int socket, op_code code){
    t_paquete *pack = crear_paquete(code);
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);

    loggear_info(loggerAux, "Kernel Thread :: Operacion confirmada", false);
}