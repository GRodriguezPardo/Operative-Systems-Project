#include "memoria.h"
#include "string.h"

void *memoria_routine(void *config){
   // t_config *config = (t_config *)arg;
    int socket;
    t_log *loggerMemoria;
    {
        char buffer[25];
        sprintf(buffer, "Cpu - Memoria Routine");
        loggerMemoria = log_create("../cpu.log", buffer, 0, LOG_LEVEL_INFO);
    }
    char* msgLog;

    char* ip = config_get_string_value(config, "IP_MEMORIA");
    char* puerto = config_get_string_value(config, "PUERTO_MEMORIA");
    socket = crear_conexion(ip, puerto);

    int __attribute__((unused)) tamanio_paquete, tamanio_restante;
    void* msg = NULL;
    t_paquete* paquete;

    op_code codigo_operacion;

    realizarHandshake(socket,loggerMemoria);

    while(1){
        sem_wait(&sem_conexion_memoria);
        switch (configMemoria->pipelineMemoria.operacion){
        case MOV_IN:
            msgLog = "LEER";
            paquete = crear_paquete(MOV_IN);
            ///TODO: Agregar a paquete el process id.
            agregar_a_paquete(paquete,(void*)&(configMemoria->pipelineMemoria.direcFisica),sizeof(uint32_t));
            enviar_paquete(paquete,socket);
            eliminar_paquete(paquete);
            break;
        case MOV_OUT:
            msgLog = "ESCRIBIR";
            paquete = crear_paquete(MOV_OUT);
            ///TODO: Agregar a paquete el process id.
            agregar_a_paquete(paquete,(void*)&(configMemoria->pipelineMemoria.direcFisica),sizeof(uint32_t));
            agregar_a_paquete(paquete,(void*)&(configMemoria->pipelineMemoria.valor),sizeof(uint32_t));
            enviar_paquete(paquete,socket);
            eliminar_paquete(paquete);
            break;
        case MMU_MARCO:
            msgLog = "BUSCAR MARCO";
            paquete = crear_paquete(MMU_MARCO);
            ///TODO: Agregar a paquete el process id.
            agregar_a_paquete(paquete,(void*)&(configMemoria->pipelineMemoria.idPagina),sizeof(uint32_t));
            agregar_a_paquete(paquete,(void*)&(configMemoria->pipelineMemoria.idTablaPagina),sizeof(uint32_t));
            enviar_paquete(paquete,socket);
            eliminar_paquete(paquete);
            break;
        default:
            pthread_mutex_lock(&mutex_logger);
            log_error(loggerMemoria,"Llego codigo desconocido a conexion con memoria");
            pthread_mutex_unlock(&mutex_logger);
            break;
        }

        pthread_mutex_lock(&mutex_logger);
        log_error(loggerMemoria,"PID: %d - Acción: %s - Segmento: %d - Pagina: %d - Dirección Fisica: %d",mi_contexto->id,msgLog,configMemoria->numSegActual,configMemoria->numPagActual,configMemoria->pipelineMemoria.direcFisica);
        pthread_mutex_unlock(&mutex_logger);

        codigo_operacion = recibir_operacion(socket);
        (void) largo_paquete(socket);
       
        switch (codigo_operacion)
        {
        case PAGE_FAULT:
            configMemoria->pipelineMemoria.operacion = PAGE_FAULT;
            break;
        case MMU_MARCO:
            configMemoria->pipelineMemoria.operacion = MMU_MARCO;
            msg = recibir(socket);
            configMemoria->numMarco = *((uint32_t*)msg);
            free(msg);
            msg = NULL;
            break;
        case MOV_IN_VALOR:
            configMemoria->pipelineMemoria.operacion = MOV_IN_VALOR;
            configMemoria->pipelineMemoria.valor = *((uint32_t*)msg);
            msg = recibir(socket);
            free(msg);
            msg = NULL;
            break;
        case MOV_OUT_CONFIRMACION:
            configMemoria->pipelineMemoria.operacion = MOV_OUT_CONFIRMACION;
            break;      
        default:
            pthread_mutex_lock(&mutex_logger);
            log_error(loggerMemoria,"Recibi codigo desconocido a conexion con memoria");
            pthread_mutex_unlock(&mutex_logger);
            break;
        }
        
        sem_post(&sem_mmu);
    }

    
    /*
    while(true)

    1. esperar a que el hilo CICLO avise que hay una instruccion de memoria
        WAIT(SEM_MOV_PEDIDO);
    2. leer de algun buffer la instruccion 
        (MOV_IN || MOV_OUT) + data del proceso (pcb || tabla_segmentos)
    3. mmu -> traducir direccion logica
        3.1. obtener num_pagina
            num_segmento = floor(dir_logica / tam_max_segmento)
            desplazamiento_segmento = dir_logica % tam_max_segmento
            num_pagina = floor(desplazamiento_segmento  / tam_pagina)
            offset_pagina = desplazamiento_segmento % tam_pagina
        3.2. pedir a memoria el num_marco
        3.3. obtener direccion fisica 
            (num_marco * tam_pagina + offset_pagina)
    4. enviar instruccion a memoria
        [MOV_IN || (MOV_OUT + valor)] && direccion fisica && data del proceso (para chequear el reemplazo local)
    5. esperar respuesta
        5.a. OK -> notificar a hilo CICLO 
                    SIGNAL(SEM_MOV_REALIZADO)
        5.b. PAGE_FAULT -> notificar ¿CICLO? para desalojar el proceso 

    
    int *ret = (int *)malloc(sizeof(int));
    pthread_exit(ret);*/
}

void realizarHandshake(int socket, t_log* logger){
    t_paquete *pack = crear_paquete(INIT_CPU);
    int valor = 0;
    agregar_a_paquete(pack, &valor, sizeof(int));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
    
    //recibo respuesta
    op_code code = recibir_operacion(socket);
    if(code != INIT_MEMORIA){
            perror("Error: Conexion con memoria no recibio mensaje esperado");
            logger_cpu_error(logger, "Error: Conexion con memoria recibio un mensaje inesperado en el handshake.");
            exit(EXIT_FAILURE);
    }


    int __attribute__((unused)) tamanio = largo_paquete(socket);

    //recibo los valores de configuracion de mmu
    //recibo la cantidad de entradas por tabla de paginas
    configMemoria->entradasTablaPaginas = recibir_uint32t(socket);
    //recibo el tamaño de pagina
    configMemoria->tamanioPagina = recibir_uint32t(socket);
    configMemoria->tamanioMaximoSegmento = configMemoria->entradasTablaPaginas * configMemoria->tamanioPagina;
}

uint32_t recibir_uint32t(int socket){
    uint32_t valor;
    void *buffer = recibir(socket);
    valor = *((uint32_t *)buffer);
    free(buffer);
    return valor;
}