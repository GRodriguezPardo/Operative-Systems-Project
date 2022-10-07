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
t_contexto mi_contexto;
int main(){
    t_config *config;
    config = config_create("../cpu.config");
    printf("%s\n", config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
    t_log *logger = log_create("./cpu.log", "CPU - Main", 0, LOG_LEVEL_INFO);
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

void* ciclo_instruccion(void* config){//hace falta el config?
    sem_wait(&sem_ciclo_instruccion);
    t_log *logger_cpu_ciclo = log_create("./cpu.log", "CPU - Ciclo_instruccion", 0, LOG_LEVEL_INFO);
    ////////////// FETCH //////////////
    long retardo_instruccion = config_get_long_value((t_config*) config,"RETARDO_INSTRUCCION");
    char* instruccion;
    {
        instruccion = mi_contexto->instrucciones[mi_contexto->program_counter];
    }
    ////////////// DECODE //////////////
    char op[10], 
    oper1[10], 
    oper2[10];
    uint8_t instruction_code;
    uint8_t register1,register2;
    {
        char unSring[20] = "I/O DISCO BX";
        sscanf(unSring, "%s %s %s", op, oper1, oper2);
        //voy a realizar solucion simplista, pero podriamos solucionarlo 
        //de manera que cada instruccion sea una funcion diferente y devuelva un status.
        if(strcmp(op,"SET")==0){
            if(strcmp(oper1,"AX")==0){
                register1 = 0;
            } else if(strcmp(oper1,"BX")==0){
                register1 = 1;
            }else if(strcmp(oper1,"CX")==0){
                register1 = 2;
            }else if(strcmp(oper1,"DX")==0){
                register1 = 3;
            }else{
                logger_monitor_error(logger_cpu_ciclo,"variable invalida, devolviendo el contexto");
                sem_post(&sem_envio_contexto);
            }
            //if((int)register2 ) validar variables numericas                    
            sleep(retardo_instruccion);
            instruction_code = 0;
        }else if(strcmp(op,"ADD")){
            if(strcmp(oper1,"AX")==0){
                register1 = 0;
            } else if(strcmp(oper1,"BX")==0){
                register1 = 1;
            }else if(strcmp(oper1,"CX")==0){
                register1 = 2;
            }else if(strcmp(oper1,"DX")==0){
                register1 = 3;
            }else{
                logger_monitor_error(logger_cpu_ciclo,"variable invalida, devolviendo el contexto");
                sem_post(&sem_envio_contexto);
            }
            if(strcmp(oper2,"AX")==0){
                register1 = 0;
            } else if(strcmp(oper2,"BX")==0){
                register1 = 1;
            }else if(strcmp(oper2,"CX")==0){
                register1 = 2;
            }else if(strcmp(oper2,"DX")==0){
                register1 = 3;
            }else{
                logger_monitor_error(logger_cpu_ciclo,"variable invalida, devolviendo el contexto");
                sem_post(&sem_envio_contexto);
            }
            sleep(retardo_instruccion);
            instruction_code = 1;
        }else if(strcmp(op,"MOV_IN")){
            instruction_code = 2;
        }else if(strcmp(op,"MOV_OUT")){
            instruction_code = 3;
        }else if(strcmp(op,"I/0")){
            instruction_code = 4;
        }else if(strcmp(op,"EXIT")){
            instruction_code = 5;
        }else{
            perror("instruccion invalida, devolviendo el contexto");
            sem_post(&sem_envio_contexto);
        }
    }
    ////////////// EXECUTE //////////////
    {
        switch(instruction_code){
            case 0://set
                mi_contexto->registros[register1] = (uint32_t)oper2;
                break;
            case 1://add
                mi_contexto->registros[register1] += mi_contexto->registros[register2];
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                sem_post(&sem_envio_contexto);// Se deberá devolver el Contexto de Ejecución actualizado al Kernel junto el dispositivo y la cantidad 
                //de unidades de trabajo del dispositivo que desea utilizar el proceso 
                break;
            case 5:
                sem_post(&sem_envio_contexto);
                break;
            default:
        }
    }
    ////////////// CHECK INTERRUPT //////////////
    {

    }
    mi_contexto->program_counter++;
}

void* interrupt_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_INTERRUPT");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de interrupt.\n");
    iniciar_servidor(ipCPU,puertoServidor,interrupt_routine);
}

void* dispatch_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_DISPATCH");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de dispatch.\n");
    iniciar_servidor(ipCPU,puertoServidor,dispatch_routine);
}


void *dispatch_routine(void* socket){
    //logica consumidor - productor, semaforo que se libera por kernel
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;
    ////////////// DECODING PARAM //////////////
    int socket_dispatch = *(int *)socket;
    op_code codigo_operacion;
    
    
    int __attribute__((unused)) tamanio_paquete, tamanio_restante;
    void* msg = NULL;
    t_paquete* paquete;
    
    ////////////// CREANDO LOGGER DEL HANDLER //////////////
    t_log *logger;
    {
        char buffer[25];
        sprintf(buffer, "Cpu - Kernel Dispatch");
        logger = log_create("./kernel.log", buffer, 0, LOG_LEVEL_INFO);
    }
    while(1){
        codigo_operacion = recibir_operacion(socket_dispatch);
        tamanio_paquete = tamanio_restante = largo_paquete(socket_dispatch);

        

        if(codigo_operacion != CONTEXTO){
            perror("Error: Conexion con dispatch no recibio un contexto de ejecucion");
            logger_monitor_error(logger, "Error: Inicio de kernel dispatch con codigo inesperado. Finalizando hilo.");
            exit(EXIT_FAILURE);
        }

        uint32_t id;
        msg = recibir(socket_dispatch);
        id = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        ////////////// Recibiendo PC //////////////
        uint32_t pc;
        msg = recibir(socket_dispatch);
        pc = *((uint32_t*)msg);
        free(msg);
        msg = NULL;
        ////////////// Recibiendo Registros //////////////
        uint32_t registros[4];
        tamanio_restante -= 8;
        {
            for(size_t i = 0; i<4; i++){
                msg = recibir(socket_dispatch);
                registros[i] = *((uint32_t *)msg);
                free(msg);
                msg = NULL;
            }
            tamanio_restante -=32;
        }
        ////////////// Recibiendo Segmentos //////////////
        uint32_t segmentos[4][2];
        {
            for(size_t i = 0;i<4;i++){
                msg = recibir(socket_dispatch);
                segmentos[i][0] = *((uint32_t *)msg);//tamanio
                free(msg);
                msg = NULL;
                msg = recibir(socket_dispatch);
                segmentos[i][1] = *((uint32_t *)msg);//numero tabla paginas
                free(msg);
                msg = NULL;
            }
        }
        ////////////// Recibiendo Instrucciones//////////////
        char **instrucciones;
        uint32_t cantidad = 0;
        //uint32_t cantidad = 0;
        {
            while(tamanio_restante > 0)
            {
                msg = recibir(socket_dispatch);
                instrucciones = (char **)realloc(instrucciones, sizeof(char *) * (cantidad + 1));
                instrucciones[cantidad] = msg;
                msg = NULL;

                tamanio_restante -= (strlen(instrucciones[cantidad]) + 1 + sizeof(int));
                cantidad++;
            }
        }
        ////////////// Creando contexto //////////////
        
        {
            mi_contexto->id = id;
            mi_contexto->program_counter = pc;
            mi_contexto->instrucciones = instrucciones;
            for(size_t i = 0;i<4;i++){
                mi_contexto->registros[i] = registros[i];
            }
            for(size_t i = 0;i<4;i++){
                mi_contexto->segmentos[i][0] = segmentos[i][0];
                mi_contexto->segmentos[i][1] = segmentos[i][1];
            }
        }

        sem_post(&sem_ciclo_instruccion);
        ////////////// Esperando para enviar contexto //////////////
        sem_wait(&sem_envio_contexto);//este sem se frena para esperar la devolucion del contexto
        logger_cpu_info(logger,"realizando envio contexto");
        paquete = crear_paquete(CONTEXTO);
        agregar_a_paquete(paquete,(void *)mi_contexto->id,sizeof(uint32_t));
        agregar_a_paquete(paquete,(void *)mi_contexto->program_counter,sizeof(uint32_t));
        for(size_t i = 0; i<4;i++)
        {
            agregar_a_paquete(paquete,(void *)mi_contexto->registros[i],sizeof(uint32_t));
        }
        for(size_t i = 0; i<4; i++){
            agregar_a_paquete(paquete,(void *)mi_contexto->segmentos[i][0],sizeof(uint32_t));
            agregar_a_paquete(paquete,(void *)mi_contexto->segmentos[i][1],sizeof(uint32_t));
        }
        for(size_t i = 0;i<cantidad;i++){
            agregar_a_paquete(paquete,(void *)mi_contexto->instrucciones[i],strlen(mi_contexto->instrucciones[i])+1);
        }
        enviar_paquete(paquete,socket_dispatch);
        eliminar_paquete(paquete);
    }
        
    pthread_exit(return_status);
}

void *interrupt_routine(void* socket){
    int *return_status = (int*)malloc(sizeof(int));
    *return_status = 0;

    int socket_cliente = *(int *)socket;
    op_code codigo_operacion;
    int tamaño_paquete;
    void* msg;
    t_paquete* paquete;

    while (1)
    {
        codigo_operacion = recibir_operacion(socket_cliente);
        tamaño_paquete = largo_paquete(socket_cliente);

        switch(codigo_operacion) {
            case MENSAJE:
                msg = recibir(socket_cliente);
                printf("Recibi el mensaje: %s\nEn el socket: %d\n", (char*) msg, socket_cliente);

                char* respuesta = "Recibido";
                paquete = crear_paquete(RESPUESTA);
                agregar_a_paquete(paquete, (void*) respuesta, strlen(respuesta) + 1);
                enviar_paquete(paquete, socket_cliente);
                eliminar_paquete(paquete);

                free(msg);
                break;
            default:
                perror("Recibí una operacion inesperada. Terminando programa.\n");
                *return_status = 1;
                pthread_exit(return_status);
                break;          
        }
    }
    pthread_exit(return_status);
}
