#include "dispatch.h"

void* dispatch_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_DISPATCH");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de dispatch.\n");
    iniciar_servidor(ipCPU,puertoServidor,dispatch_routine);

    exit(EXIT_SUCCESS);
    return NULL;
}


void *dispatch_routine(void* socket){
    //logica consumidor - productor, semaforo que se libera por kernel
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    ////////////// DECODING PARAM //////////////
    int socket_dispatch = *(int *)socket;
    op_code codigo_operacion;
    uint32_t id;
    uint32_t pc;
    uint32_t registros[4];
    t_segmento *segmentos;
    uint32_t cantidad = 0;
    char **instrucciones;
    uint32_t cantSegmentos = 0;


    int __attribute__((unused)) tamanio_paquete, tamanio_restante;
    void* msg = NULL;
    t_paquete* paquete;
    
    ////////////// CREANDO LOGGER DEL HANDLER //////////////
    t_log *logger;
    {
        char buffer[25];
        sprintf(buffer, "Cpu - Kernel Dispatch");
        logger = log_create("../cpu.log", buffer, 0, LOG_LEVEL_INFO);
    }
    
    
    paquete = crear_paquete(INIT_CPU);
    enviar_paquete(paquete,socket_dispatch);
    eliminar_paquete(paquete);
    
     
    while(1){
        codigo_operacion = recibir_operacion(socket_dispatch);
        tamanio_paquete = tamanio_restante = largo_paquete(socket_dispatch);

        if(codigo_operacion != PROXIMO_PCB){
            perror("Error: Conexion con dispatch no recibio un contexto de ejecucion");
            logger_cpu_error(logger, "Error: Inicio de kernel dispatch con codigo inesperado. Finalizando hilo.");
            exit(EXIT_FAILURE);
        }

        mi_contexto->pipeline.operacion = PROXIMO_PCB;
        msg = recibir(socket_dispatch);
        id = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        ////////////// Recibiendo PC //////////////
        msg = recibir(socket_dispatch);
        pc = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        ////////////// Recibiendo Registros //////////////
        msg = recibir(socket_dispatch);
        for(size_t i = 0; i<4; i++){
            registros[i] = ((uint32_t *)msg)[i];
        }
        free(msg);
        msg = NULL;
        ////////////// Recibiendo Segmentos //////////////
        msg = recibir(socket_dispatch);
        cantSegmentos = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        msg = recibir(socket_dispatch);
        segmentos = (t_segmento*)msg;
        msg = NULL;
        ////////////// Recibiendo Instrucciones//////////////
        msg = recibir(socket_dispatch);
        cantidad = *((uint32_t *)msg);
        free(msg);
        msg = NULL;
        {
            instrucciones = (char**)malloc(sizeof(char *) * (cantidad));
            for(uint32_t i = 0; i <cantidad; i++)
            {
                msg = recibir(socket_dispatch);
                instrucciones[i] = msg;
                msg = NULL;
            }
        }
        ////////////// Actualizando contexto //////////////
        pthread_mutex_lock(&mutex_logger);
        log_info(logger,"%s",instrucciones[0]);
        pthread_mutex_unlock(&mutex_logger);
        {
            mi_contexto->id = id;
            mi_contexto->program_counter = pc;
            mi_contexto->instrucciones = instrucciones;
            for(size_t i = 0;i<4;i++){
                mi_contexto->registros[i] = registros[i];
            }
            mi_contexto->segmentos = segmentos;
            pthread_mutex_lock(&mutex_flag);
            pid_interrupt = -1;
            flag_interrupcion = 0;
            pthread_mutex_unlock(&mutex_flag);
        }

        sem_post(&sem_ciclo_instruccion);
        
        sem_wait(&sem_envio_contexto);
        ////////////// Esperando para enviar contexto //////////////
        logger_cpu_info(logger,"realizando envio contexto");
        
        switch(mi_contexto -> pipeline.operacion){
            case DESALOJO_PROCESO:
                paquete = crear_paquete(mi_contexto -> pipeline.operacion);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->id),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->program_counter),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->registros),sizeof(uint32_t)*4);
                agregar_a_paquete(paquete,(void *)(mi_contexto->segmentos),sizeof(t_segmento)*cantSegmentos);
                enviar_paquete(paquete,socket_dispatch);
                eliminar_paquete(paquete);
                for(size_t i = 0; i < cantidad; i++){
                    free(instrucciones[i]);
                }
                free(instrucciones);
                free(segmentos);
                break;
            case EXIT_PROCESO:
                paquete = crear_paquete(mi_contexto -> pipeline.operacion);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->id),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->program_counter),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->registros),sizeof(uint32_t)*4);
                agregar_a_paquete(paquete,(void *)(mi_contexto->segmentos),sizeof(t_segmento)*cantSegmentos);
                enviar_paquete(paquete,socket_dispatch);
                eliminar_paquete(paquete);
                for(size_t i = 0; i < cantidad; i++){
                    free(instrucciones[i]);
                }
                free(instrucciones);
                free(segmentos);
                break;
            case BLOQUEO_PROCESO:
                paquete = crear_paquete(mi_contexto -> pipeline.operacion);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->id),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->program_counter),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->registros),sizeof(uint32_t)*4);
                agregar_a_paquete(paquete,(void *)(mi_contexto->segmentos),sizeof(t_segmento)*cantSegmentos);
                agregar_a_paquete(paquete,(void *)(mi_contexto->dispositivo),strlen(mi_contexto->dispositivo)+1);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->unidades),sizeof(uint32_t));
                enviar_paquete(paquete,socket_dispatch);
                eliminar_paquete(paquete);
                for(size_t i = 0; i < cantidad; i++){
                    free(instrucciones[i]);
                }
                free(instrucciones);
                free(segmentos);
                break;
            default:
                pthread_mutex_lock(&mutex_logger);
                log_error(logger,"Llego codigo desconocido a devolver el contexto, Dispatch");
                pthread_mutex_unlock(&mutex_logger);
                break;
        }
        
    }  
    pthread_exit(return_status);
}