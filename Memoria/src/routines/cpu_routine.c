#include "cpu_routine.h"

void cpu_routine(int socketFd, int *returnStatus)
{
    uint32_t direccionFisica;

    recibir_handshake(socketFd, INIT_CPU);
    responder_handshakeCPU(socketFd);

    while (1)
    {
        op_code codPeticion = recibir_operacion(socketFd);
        (void)largo_paquete(socketFd);
        uint32_t pid = recibir_uint32t(socketFd);

        aplicar_retardo(ConfigMemoria.retardoMemoria);
        switch (codPeticion)
        {
        /* QUE PASA SI MMU SACA LA DIRECCION FISICA DIRECTAMENTE DESDE TLB Y ESE MARCO FUE REEMPLAZADO ---> RTA: Page Fault */
        case MOV_IN:
            direccionFisica = recibir_uint32t(socketFd); // recibo la direccion
            uint32_t valorLeido = leer_memoria(direccionFisica);
            responder_cpu(socketFd, MOV_IN_VALOR, &valorLeido);

            marcarPaginaUsada(direccionFisica, false);
            char *msgLeer = string_from_format("Acceso a espacio de usuario: PID: %d - Acción: LEER - Dirección física: %d", pid, direccionFisica);
            loggear_info(loggerMain, msgLeer, true);

            msgLeer = string_from_format("CPU :: PID: %u - Acción: LEER - Dirección física: %u - Valor Leido: %u", pid, direccionFisica, valorLeido);
            loggear_info(loggerAux, msgLeer, true);
            break;
        case MOV_OUT:
            direccionFisica = recibir_uint32t(socketFd);         // recibo la direccion
            uint32_t valorAEscribir = recibir_uint32t(socketFd); // recibo el valor a escribir
            escribir_memoria(direccionFisica, valorAEscribir);
            responder_cpu(socketFd, MOV_OUT_CONFIRMACION, NULL);

            marcarPaginaUsada(direccionFisica, true);
            char *msgEscribir = string_from_format("Acceso a espacio de usuario: PID: %d - Acción: ESCRIBIR - Dirección física: %d", pid, direccionFisica);
            loggear_info(loggerMain, msgEscribir, true);

            msgEscribir = string_from_format("CPU :: PID: %u - Acción: ESCRIBIR - Dirección física: %u - Valor Escrito: %u", pid, direccionFisica, valorAEscribir);
            loggear_info(loggerAux, msgEscribir, true);
            break;
        case MMU_MARCO:
            uint32_t numPagina = recibir_uint32t(socketFd);
            uint32_t idTabla = recibir_uint32t(socketFd);
            uint32_t marco;

            if (pag_obtenerMarcoPagina(pid, idTabla, numPagina, &marco) == -1)
            {
                responder_cpu(socketFd, PAGE_FAULT, NULL);

                char *msg = string_from_format("Acceso a Tabla de Páginas: PID: %u - Página: %u -> PAGE FAULT", pid, numPagina);
                loggear_info(loggerAux, msg, true);
            }
            else
            {
                responder_cpu(socketFd, MMU_MARCO, &marco);

                char *msg = string_from_format("Acceso a Tabla de Páginas: PID: %u - Página: %u - Marco: %u", pid, numPagina, marco);
                loggear_info(loggerMain, msg, true);
            }

            break;
        default:
            loggear_error(loggerAux, "CPU :: Operacion desconocida", false);

            pthread_mutex_unlock(&mx_main);
            exit(EXIT_FAILURE);
            break;
        }
    }
}

void responder_handshakeCPU(int socket)
{
    // mandar entradasXtablaPaginas y tamanioPagina
    t_paquete *pack = crear_paquete(INIT_MEMORIA);
    agregar_a_paquete(pack, (void *)&(ConfigMemoria.paginasPorTabla), sizeof(ConfigMemoria.paginasPorTabla));
    agregar_a_paquete(pack, (void *)&(ConfigMemoria.tamanioPagina), sizeof(ConfigMemoria.tamanioPagina));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);

    char *msg = string_from_format("CPU :: entradasPorTabla: %u // tamanioPagina: %u enviados.", ConfigMemoria.paginasPorTabla, ConfigMemoria.tamanioPagina);
    loggear_info(loggerAux, msg, true);
}

// cpu manda la dirección fisica

uint32_t leer_memoria(uint32_t offset)
{
    uint32_t valorLeido;
    pthread_mutex_lock(&mx_espacioUsuario);
    valorLeido = *(uint32_t *)(EspacioUsuario + offset);
    pthread_mutex_unlock(&mx_espacioUsuario);
    return valorLeido;
}

void escribir_memoria(uint32_t offset, uint32_t valor)
{
    pthread_mutex_lock(&mx_espacioUsuario);
    memcpy(EspacioUsuario + offset, &valor, sizeof(uint32_t));
    pthread_mutex_unlock(&mx_espacioUsuario);
}

void marcarPaginaUsada(uint32_t direccion, bool esEscritura)
{
    t_pagina *paginaUsada;

    {
        uint32_t numFrame = direccion / ConfigMemoria.tamanioPagina;
        char *sNumFrame = string_itoa(numFrame);
        pthread_mutex_lock(&mx_tablaFrames);
        paginaUsada = dictionary_get(TablaFrames, sNumFrame);
        pthread_mutex_unlock(&mx_tablaFrames);
        free(sNumFrame);
    }

    paginaUsada->usado = true;
    if(esEscritura)
        paginaUsada->modificado = true;
}

void responder_cpu(int socket, op_code code, uint32_t *valor)
{
    t_paquete *pack = crear_paquete(code);
    if (valor != NULL)
        agregar_a_paquete(pack, (void *)valor, sizeof(uint32_t));

    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
}