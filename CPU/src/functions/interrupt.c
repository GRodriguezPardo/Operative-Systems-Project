#include "interrupt.h"


void* interrupt_server(void* config){
    char* puertoServidor = config_get_string_value((t_config*) config, "PUERTO_ESCUCHA_INTERRUPT");
    char* ipCPU = config_get_string_value((t_config*) config,"IP_CPU");
    printf("Creando server de interrupt.\n");
    iniciar_servidor(ipCPU,puertoServidor,interrupt_routine);

    exit(EXIT_SUCCESS);
    return NULL;
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
        logger = log_create("../cpu.log", buffer, 0, LOG_LEVEL_INFO);
    }

    /////////// Recibiendo Interrupciones //////////
    while (1)
    {
        codigo_operacion = recibir_operacion(socket_cliente);
        int __attribute__((unused)) tamanio = largo_paquete(socket_cliente);
        switch(codigo_operacion) {
            case DESALOJO_PROCESO:
                msg = recibir(socket_cliente);
                
                pthread_mutex_lock(&mutex_logger);
                log_info(logger,"Recibi el mensaje: %d\nEn el socket: %d\n", *(int*) msg, socket_cliente);
                pthread_mutex_unlock(&mutex_logger);
                
                pthread_mutex_lock(&mutex_flag);
                pid_interrupt = *((uint32_t*)msg);
                flag_interrupcion = 1;
                pthread_mutex_unlock(&mutex_flag);

                free(msg);
                break;
            default:
                pthread_mutex_lock(&mutex_logger);
                log_error(logger,"recibi codigo de operacion invalido en interrupt, cerrando el hilo");
                pthread_mutex_unlock(&mutex_logger);
                *return_status = 1;
                pthread_exit(return_status);
                break;          
        }
    }
    pthread_exit(return_status);
}
