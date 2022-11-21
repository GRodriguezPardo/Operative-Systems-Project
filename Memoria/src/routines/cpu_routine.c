#include "cpu_routine.h"

void cpu_routine(int socketFd, int *returnStatus){
    uint32_t valorRespuesta = NULL;
    op_code codRespuesta;
    uint32_t direccionFisica;

    recibir_handshake(socketFd, INIT_CPU);
    responder_handshakeCPU(socketFd);

    while(1){
        op_code codPeticion = recibir_operacion(socketFd);
        int __attribute_maybe_unused__ tamanio = largo_paquete(socketFd);
        uint32_t pid = recibir_uint32t(socketFd);

        aplicar_retardo(configMemoria.retardoMemoria);
        switch (codPeticion)
        {
        /* QUE PASA SI MMU SACA LA DIRECCION FISICA DIRECTAMENTE DESDE TLB Y ESE MARCO FUE REEMPLAZADO ---> RTA: Page Fault */
        case MOV_IN:
            codRespuesta = MOV_IN_VALOR;
            direccionFisica = recibir_uint32t(socketFd); //recibo la direccion
            valorRespuesta = leer_memoria(direccionFisica);
            //LOGUEAR
            break;
        case MOV_OUT:
            codRespuesta = MOV_OUT_CONFIRMACION;
            direccionFisica = recibir_uint32t(socketFd); //recibo la direccion
            uint32_t valorAEscribir = recibir_uint32t(socketFd); //recibo el valor a escribir
            escribir_memoria(direccionFisica, valorAEscribir);
            //LOGUEAR
            break;
        case MMU_MARCO:
            codRespuesta = MMU_MARCO;
            uint32_t numPagina = recibir_uint32t(socketFd);
            uint32_t idTabla = recibir_uint32t(socketFd);
            uint32_t marco;

            if (pag_obtenerMarcoPagina(idTabla, numPagina, &marco) == -1){
                codRespuesta = PAGE_FAULT;
                //AGREGO LA INFO DE LA PAGINA PARA BUSCARLA CUANDO KERNEL PIDA EL SWAP
                t_pagina_page_fault *pagPF = (t_pagina_page_fault *) malloc(sizeof(t_pagina_page_fault));
                pagPF->idProceso = pid;
                pagPF->idTabla = idTabla;
                pagPF->numPagina = numPagina;
                insertar_pagina_pageFault(ListaPageFaults, pagPF);
            }
            else 
                valorRespuesta = marco;
            
            //LOGUEAR
            break;
        default:
            *returnStatus = EXIT_FAILURE;
            pthread_mutex_unlock(&mx_main);
            pthread_exit(returnStatus);
            break;
        }

        responder_cpu(socketFd, codRespuesta, valorRespuesta);
    }

    *returnStatus = EXIT_SUCCESS;
    pthread_mutex_unlock(&mx_main);
    pthread_exit(returnStatus);
}

static void responder_handshakeCPU(int socket){
    //mandar entradasXtablaPaginas y tamanioPagina
    t_paquete *pack = crear_paquete(INIT_MEMORIA);
    agregar_a_paquete(pack, (void *)&(configMemoria.entradasPorTabla), sizeof(configMemoria.entradasPorTabla));
    agregar_a_paquete(pack, (void *)&(configMemoria.tamanioPagina), sizeof(configMemoria.tamanioPagina));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);

    char *msg = string_from_format("CPU Thread :: entradasPorTabla: %d // tamanioPagina: %d enviados.", configMemoria.entradasPorTabla, configMemoria.tamanioPagina);
    loggear_info(logger, msg);
    free(msg);
}

// cpu manda la dirección fisica

uint32_t leer_memoria(uint32_t offset){
    uint32_t valorLeido;
    pthread_mutex_lock(&mx_espacioUsuario);
    valorLeido = *(uint32_t *)(EspacioUsuario + offset);
    pthread_mutex_unlock(&mx_espacioUsuario);
    return valorLeido;
}

void escribir_memoria(uint32_t offset, uint32_t valor){
    pthread_mutex_lock(&mx_espacioUsuario);
    memcpy(EspacioUsuario + offset, &valor, sizeof(uint32_t));
    pthread_mutex_unlock(&mx_espacioUsuario);
}

void insertar_pagina_pageFault(t_list *listaPageFaults, t_pagina_page_fault *pagPF){
    pthread_mutex_lock(&mx_listaPageFaults);
    list_add(listaPageFaults, pagPF);
    pthread_mutex_unlock(&mx_listaPageFaults);
}

void responder_cpu(int socket, op_code code, uint32_t valor){
    t_paquete *pack = crear_paquete(code);
    if(valor != NULL)
        agregar_a_paquete(pack, (void *)&valor, sizeof(valor));

    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
}