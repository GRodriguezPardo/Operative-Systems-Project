#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <thesenate/tcp_serializacion.h>
#include <thesenate/tcp_server.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include "cpu_utils.h"
#include "cpu.h"


sem_t sem,sem_ciclo_instruccion,sem_envio_contexto;

int main(){
    t_config *config;
    config = config_create("../cpu.config");
    printf("%s\n", config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
    t_log *logger = log_create("../cpu.log", "CPU - Main", 0, LOG_LEVEL_INFO);
    init_globals_cpu();
    pthread_t interrupt, dispatch, executer;
    if (pthread_create(&interrupt, NULL, interrupt_server, (void *) config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }

    if (pthread_create(&dispatch, NULL, dispatch_server, (void *) config) < 0)
    {
        perror("Error: Memoria thread failed."); // cambiar a los logs
    }
    
    if (pthread_create(&executer, NULL,ciclo_instruccion, (void *) config) < 0)
    {
        perror("Error: ciclo instruccion thread failed."); // cambiar a los logs
    }

    sem_init(&sem, 1, 0);
    sem_wait(&sem);
    finalizar_cpu(config,logger);

    return 0;
}

void* ciclo_instruccion(void* config){
    
    t_log *logger_cpu_ciclo = log_create("../cpu.log", "CPU - Ciclo_instruccion", 0, LOG_LEVEL_INFO);
    long retardo_instruccion = config_get_long_value((t_config*) config,"RETARDO_INSTRUCCION");
    char op[10], 
    oper1[10], 
    oper2[10];
    char* instruccion;
    uint8_t instruction_code;
    uint8_t register1,register2;
    bool devolverContexto;
    ////////////// FETCH //////////////
    sem_wait(&sem_ciclo_instruccion);
    log_info(logger_cpu_ciclo,"%s",mi_contexto->instrucciones[mi_contexto->program_counter]);
    while(1){
            {
            instruccion = mi_contexto->instrucciones[mi_contexto->program_counter];
            }
        ////////////// DECODE //////////////
            log_info(logger_cpu_ciclo,"%s",instruccion);
            {
                sscanf(instruccion, "%s %s %s", op, oper1, oper2);
                log_info(logger_cpu_ciclo,"la operacion es :%s",op);
                if(!strcmp(op,"SET")){
                    if(!strcmp(oper1,"AX")){
                        register1 = 0;
                    } else if(!strcmp(oper1,"BX")){
                        register1 = 1;
                    }else if(!strcmp(oper1,"CX")){
                        register1 = 2;
                    }else if(!strcmp(oper1,"DX")){
                        register1 = 3;
                    }        
                    sleep(retardo_instruccion);
                    instruction_code = 0;
                }else if(!strcmp(op,"ADD")){
                    if(!strcmp(oper1,"AX")){
                        register1 = 0;
                    } else if(!strcmp(oper1,"BX")){
                        register1 = 1;
                    }else if(!strcmp(oper1,"CX")){
                        register1 = 2;
                    }else if(!strcmp(oper1,"DX")){
                        register1 = 3;    
                    }
                    if(!strcmp(oper2,"AX")){
                        register2 = 0;
                    } else if(!strcmp(oper2,"BX")){
                        register2 = 1;
                    }else if(!strcmp(oper2,"CX")){
                        register2 = 2;
                    }else if(!strcmp(oper2,"DX")){
                        register2 = 3;
                    }
                    sleep(retardo_instruccion);
                    instruction_code = 1;   
                }else if(!strcmp(op,"MOV_IN")){
                    instruction_code = 2;
                }else if(!strcmp(op,"MOV_OUT")){
                    instruction_code = 3;
                }else if(!strcmp(op,"I/0")){
                    instruction_code = 4;
                }else if(!strcmp(op,"EXIT")){
                    instruction_code = 5;
                }else{
                    perror("instruccion invalida, devolviendo el contexto");
                    devolverContexto = true;
                }
            }
            ////////////// EXECUTE //////////////
            {
                switch(instruction_code){
                    case 0://set
                        mi_contexto->registros[register1] = (uint32_t)atoi(oper2);
                        break;
                    case 1://add
                        mi_contexto->registros[register1] += mi_contexto->registros[register2];
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4://I/O
                        mi_contexto->dispositivo = oper1;
                        mi_contexto->unidades = (uint32_t)atoi(oper2);
                        devolverContexto = true;
                        mi_contexto -> pipeline.operacion = BLOQUEO_PROCESO; //pensar prioridades de razones para desalojar
                        break;
                    case 5://EXIT
                        devolverContexto = true;
                        mi_contexto -> pipeline.operacion = EXIT_PROCESO;
                        break;
                    default:
                }
            }
        mi_contexto->program_counter++;
        log_info(logger_cpu_ciclo,"PID:%d - Ejecutando: %s - %s - %s",mi_contexto->id,op,oper1,oper2);
        ////////////// CHECK INTERRUPT //////////////
        {
            if(flag_interrupcion == 1){
                if(pid_interrupt == mi_contexto->id){
                    devolverContexto = true;
                    mi_contexto -> pipeline.operacion = DESALOJO_PROCESO;
                }
            }
        }
        if(devolverContexto){
            sem_post(&sem_envio_contexto);
        }
        
    }
    
}

void* interrupt_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_INTERRUPT");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de interrupt.\n");
    iniciar_servidor(ipCPU,puertoServidor,interrupt_routine);

    exit(EXIT_SUCCESS);
    return NULL;
}

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
    t_segmento segmentos[4];
    uint32_t cantidad = 0;
    char **instrucciones;

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
        for(size_t i = 0;i<4;i++){
            segmentos[i] = ((t_segmento *)msg)[i];
        }
        free(msg);
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
                instrucciones[i] = (char*)msg;
                free(msg);
                msg = NULL;
            }
        }
        ////////////// Actualizando contexto //////////////
        log_info(logger,"%s",instrucciones[0]);
        {
            mi_contexto->id = id;
            mi_contexto->program_counter = pc;
            mi_contexto->instrucciones = instrucciones;
            for(size_t i = 0;i<4;i++){
                mi_contexto->registros[i] = registros[i];
            }
            for(size_t i = 0;i<4;i++){
                mi_contexto->segmentos[i] = segmentos[i];
            }
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
                agregar_a_paquete(paquete,(void *)&(mi_contexto->segmentos),sizeof(t_segmento)*4);
                enviar_paquete(paquete,socket_dispatch);
                eliminar_paquete(paquete);
                break;
            case EXIT_PROCESO:
                paquete = crear_paquete(mi_contexto -> pipeline.operacion);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->id),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->program_counter),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->registros),sizeof(uint32_t)*4);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->segmentos),sizeof(t_segmento)*4);
                enviar_paquete(paquete,socket_dispatch);
                eliminar_paquete(paquete);
                break;
            case BLOQUEO_PROCESO:
                paquete = crear_paquete(mi_contexto -> pipeline.operacion);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->id),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->program_counter),sizeof(uint32_t));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->registros),sizeof(uint32_t)*4);
                agregar_a_paquete(paquete,(void *)&(mi_contexto->segmentos),sizeof(t_segmento)*4);
                agregar_a_paquete(paquete,(void*)&(mi_contexto->dispositivo),strlen(mi_contexto->dispositivo));
                agregar_a_paquete(paquete,(void *)&(mi_contexto->unidades),sizeof(uint32_t));
                enviar_paquete(paquete,socket_dispatch);
                eliminar_paquete(paquete);
                break;
            default:
                log_error(logger,"Llego codigo desconocido a devolver el contexto, Dispatch");
                break;
        }
        
    }  
    pthread_exit(return_status);
}

void *interrupt_routine(void* socket){
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    pid_interrupt = 0;
    int socket_cliente = *(int *)socket;
    op_code codigo_operacion;
    void* msg;
    ///////////// Inicializando Logger /////////////
    t_log *logger;
    {
        char buffer[25];
        sprintf(buffer, "Cpu - Kernel interrupt");
        logger = log_create("../Cpu.log", buffer, 0, LOG_LEVEL_INFO);
    }

    /////////// Recibiendo Interrupciones //////////
    while (1)
    {
        codigo_operacion = recibir_operacion(socket_cliente);
        switch(codigo_operacion) {
            case DESALOJO_PROCESO:
                msg = recibir(socket_cliente);
                log_info(logger,"Recibi el mensaje: %s\nEn el socket: %d\n", (char*) msg, socket_cliente);
                pid_interrupt = *((uint32_t*)msg);
                free(msg);
                flag_interrupcion = 1;
                break;
            default:
                log_error(logger,"recibi codigo de operacion invalido en interrupt, cerrando el hilo");
                *return_status = 1;
                pthread_exit(return_status);
                break;          
        }
    }
    pthread_exit(return_status);
}
