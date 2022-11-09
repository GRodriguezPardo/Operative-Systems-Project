#include "memoria.h"
#include "string.h"

static int conectarAMemoria(char*, char*);
static void realizarHandshake(int);

t_configMemoria configMemoria;

void *memoria_com(void *arg){
    t_config *config = (t_config *)arg;

    configMemoria.ip = config_get_string_value(config, "IP_MEMORIA");
    configMemoria.puerto = config_get_string_value(config, "PUERTO_MEMORIA");

    /*int socketMemoria = conectarAMemoria(configMemoria.ip, configMemoria.puerto);
    realizarHandshake(socketMemoria);
    
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

static int conectarAMemoria(char *ip, char *puerto){
    int socketFd = crear_conexion(ip, puerto);
    return socketFd;
}

static void realizarHandshake(int socket){
    t_paquete *pack = crear_paquete(INIT_CPU);
    int valor = 0;
    agregar_a_paquete(pack, &valor, sizeof(int));
    enviar_paquete(pack, socket);
    eliminar_paquete(pack);
    
    //recibo respuesta
    op_code code = recibir_operacion(socket);
    int __attribute__((unused)) tamanio = largo_paquete(socket);

    //recibo los valores de configuracion de mmu
    //recibo la cantidad de entradas por tabla de paginas
    configMemoria.entradasTablaPaginas = recibir_uint32t(socket);
    //recibo el tamaño de pagina
    configMemoria.tamanioPagina = recibir_uint32t(socket);
    configMemoria.tamanioMaximoSegmento = configMemoria.entradasTablaPaginas * configMemoria.tamanioPagina;
}

uint32_t recibir_uint32t(int socket){
    uint32_t valor;
    void *buffer = recibir(socket);
    valor = *((uint32_t *)buffer);
    free(buffer);
    return valor;
}