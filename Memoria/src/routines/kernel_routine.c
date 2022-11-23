#include "kernel_routine.h"

static void responder_OK(int socketFd, op_code code);

void kernel_routine(int socketKernel, int *returnStatus)
{
    recibir_handshake(socketKernel, INIT_KERNEL);
    responder_OK(socketKernel, INIT_MEMORIA);

    op_code operacion;
    uint32_t pid;
    uint32_t cantidadSegmentos;
    t_segmento_pcb *segmentos = NULL;
    void *msg;

    while (1)
    {
        operacion = recibir_operacion(socketKernel);
        (void)largo_paquete(socketKernel);
        pid = recibir_uint32t(socketKernel);

        switch (operacion)
        {
        case NUEVO_PROCESO:
            cantidadSegmentos = recibir_uint32t(socketKernel);
            segmentos = (t_segmento_pcb *)recibir(socketKernel);

            t_paquete *packRespuesta = crear_paquete(NUEVO_PROCESO);

            //////////////// CREANDO INFO PROCESO ////////////////
            t_infoProceso *infoP = (t_infoProceso *)malloc(sizeof(t_infoProceso));
            infoP->tablasProceso = list_create();
            infoP->paginasPresentes = queue_create();

            //////////////// CREANDO TABLA DE PAGINAS POR SEGMENTO ////////////////
            for (uint32_t n = 0; n < cantidadSegmentos; n++)
            {
                uint32_t _idTabla = pag_crearTablaPaginas(infoP->tablasProceso, segmentos[n].tamanio);
                (segmentos[n]).identificador_tabla = _idTabla;
                {
                    char *msg = string_from_format("Creacion de Tabla de Paginas -> PID: %d - Segmento: %d - TAMAÑO: %d páginas", pid, n, ConfigMemoria.paginasPorTabla);
                    loggear_info(loggerMain, msg, true);
                }
            }
            agregar_a_paquete(packRespuesta, (void *)&pid, sizeof(uint32_t));
            agregar_a_paquete(packRespuesta, (void *)segmentos, sizeof(t_segmento_pcb) * cantidadSegmentos);

            //////////////// AGREGANDO INFO PROCESO A DICCIONARIO ////////////////
            char *sPID = string_itoa(pid);
            pthread_mutex_lock(&mx_espacioTablasPag);
            dictionary_put(EspacioTablas, sPID, infoP);
            pthread_mutex_unlock(&mx_espacioTablasPag);
            free(sPID);

            enviar_paquete(packRespuesta, socketKernel);
            eliminar_paquete(packRespuesta);

            free(segmentos);
            segmentos = NULL;

            break;
        case EXIT_PROCESO:
            liberar_proceso(pid);
            responder_OK(socketKernel, EXIT_PROCESO);
            {
                char *msg = string_from_format("Destrucción de Tabla de Paginas -> PID: %d", pid);
                loggear_info(loggerMain, msg, true);
            }
            break;
        case PAGE_FAULT:
            uint32_t idTabla = recibir_uint32t(socketKernel);
            uint32_t numPagina = recibir_uint32t(socketKernel);

            uint32_t numFrame = swap_resolver_pageFault(pid, idTabla, numPagina);
            crearEntradaTablaFrames(numFrame, pid, idTabla, numPagina);
            responder_OK(socketKernel, PAGE_FAULT);
            break;
        default:
            *returnStatus = EXIT_FAILURE;
            pthread_mutex_unlock(&mx_main);
            pthread_exit(returnStatus);
            break;
        }
    }

    *returnStatus = EXIT_SUCCESS;
    pthread_mutex_unlock(&mx_main);
    pthread_exit(returnStatus);
}

void liberar_proceso(uint32_t pid)
{
    t_infoProceso *dataP;

    char *sPID = string_itoa(pid);
    pthread_mutex_lock(&mx_espacioTablasPag);
    dataP = (t_infoProceso *)dictionary_remove(EspacioTablas, sPID);
    pthread_mutex_unlock(&mx_espacioTablasPag);
    free(sPID);

    queue_destroy(dataP->paginasPresentes);
    list_destroy_and_destroy_elements(dataP->tablasProceso, pag_destruirTablaPaginas);
    free(dataP);
}

static void responder_OK(int socket, op_code code)
{
    t_paquete *pack = crear_paquete(code);
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);

    loggear_info(loggerAux, "Kernel Thread :: Operacion confirmada", false);
}