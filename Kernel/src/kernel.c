#include "../../Libreria/tcp_server.c"
#include<pthread.h> 
#include "commons/log.h"
#include "commons/config.h"

int main(){

    pthread_t memoria,cpu_dispatch,cpu_interrupt;
    t_config* config;
    config = config_create("kernel.config");
    if(pthread_create( &memoria, NULL,  memoria_routine, (void*) &config) < 0 ){
        perror("Error: Memoria thread failed.");//cambiar a los logs
    }
    if(pthread_create( &memoria, NULL,  cpu_dispatch_routine, (void*) &config) < 0){
        perror("Error: Cpu_dispatch thread failed.");//cambiar a los logs
    }
    if(pthread_create( &memoria, NULL,  cpu_interrupt_routine, (void*) &config) < 0){
        perror("Error: Cpu_interrupt thread failed.");
    }

    iniciar_servidor("127.0.0.1","8000",consola_routine);

    return 0;
}
void* consola_routine(void* socket){
    int socket_cliente = *(int*) socket;
    
    //to do escuchar--desearlizacion
    close(socket_cliente);
    return 0;
}

void* memoria_routine(void* config){

}

void* cpu_dispatch_routine(void* config){

}
void* cpu_interrupt_routine(void* config){

}
