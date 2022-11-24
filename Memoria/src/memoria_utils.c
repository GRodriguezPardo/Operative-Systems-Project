#include "memoria_utils.h"

static uint32_t buscar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa);
static bool posicion_esta_asignada(t_bitarray *bitmap, int index);
static void marcar_posicion_asignada(t_bitarray *bitmap, int marco);


void esperar_hilos(){
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void recibir_handshake(int socketFd, op_code codigo) {
    char *msg = string_from_format("Se conectó %s.", (codigo == INIT_KERNEL) ? "Kernel" : "CPU");
    loggear_info(loggerAux, msg, true);

    int __attribute__((unused)) tamanio = largo_paquete(socketFd);
    loggear_info(loggerAux, "ACK Recibido", false);
}

void loggear_info(t_log *log, char *msg, bool freeMsg){
    pthread_mutex_lock(&mx_loggerAux);
    log_info(log, msg);
    pthread_mutex_unlock(&mx_loggerAux);

    if(freeMsg) free(msg);
}

void loggear_error(t_log *log, char *msg, bool freeMsg){
    pthread_mutex_lock(&mx_loggerAux);
    log_error(log, msg);
    pthread_mutex_unlock(&mx_loggerAux);

    if(freeMsg) free(msg);
}

void aplicar_retardo(uint32_t tiempo_ms){
    usleep(tiempo_ms * 1000); //tiempo_ms está en milisegundos
}

uint32_t recibir_uint32t(int socket){
    uint32_t valor;
    void *buffer = recibir(socket);
    valor = *((uint32_t *)buffer);
    free(buffer);
    return valor;
}

int sonIguales(const char* str1, const char* str2){
    return !strcmp(str1, str2);
}

t_infoProceso *get_info_proceso(uint32_t pid){
    t_infoProceso *dataP;

    char *sPID = string_itoa(pid);
    pthread_mutex_lock(&mx_espacioKernel);
    dataP = (t_infoProceso *)dictionary_get(EspacioKernel, sPID);
    pthread_mutex_unlock(&mx_espacioKernel);
    free(sPID);

    return dataP;
}

t_pagina *get_pagina(uint32_t pid, uint32_t idTabla, uint32_t numPagina){
    t_infoProceso *dataP = get_info_proceso(pid);
    t_list *tablaActual = list_get(dataP->tablasProceso, idTabla);
    t_pagina *pagina = list_get(tablaActual, numPagina);
    return pagina;
}

uint32_t asignar_frame_libre(){
    return asignar_posicion_libre(MapaFrames, ConfigMemoria.cantidadMarcosMemoria);
}

uint32_t asignar_slot_swap_libre(){
    return asignar_posicion_libre(MapaSwap, ConfigMemoria.cantidadPaginasSwap);
}

uint32_t asignar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa){
    uint32_t pos = buscar_posicion_libre(bitmap, longitudMapa);
    marcar_posicion_asignada(bitmap, pos);
    return pos;
}

void liberar_posicion(t_bitarray *bitmap, uint32_t index){
    bitarray_clean_bit(bitmap, index);
}

static uint32_t buscar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa){
    uint32_t posLibre = UINT32_MAX;
    for (uint32_t i = 0; i < longitudMapa; i++)
    {
        if(!posicion_esta_asignada(bitmap, i)){
            posLibre = i;
            break;
        }
    }
    return posLibre;
}

static bool posicion_esta_asignada(t_bitarray *bitmap, int index){
    return bitarray_test_bit(bitmap, index);
}

static void marcar_posicion_asignada(t_bitarray *bitmap, int index){
    bitarray_set_bit(bitmap, index);
}

void crearEntradaTablaFrames(uint32_t numFrame, uint32_t pid, uint32_t idTabla, uint32_t numPagina){
    t_pagina *paginaNueva = get_pagina(pid, idTabla, numPagina);
    
    char *sNumFrame = string_itoa(numFrame);
    dictionary_put(TablaFrames, sNumFrame, paginaNueva);
    free(sNumFrame);

    char *msg = string_from_format("Creando entrada en Tabla de Frames -> Frame: %u PID: %u Segmento: %u Pagina: %u", numFrame, pid, paginaNueva->numeroSegmento, numPagina);
    loggear_info(loggerAux, msg, true);
}

void borrarEntradaTablaFrames(uint32_t numFrame){
    char *sNumFrame = string_itoa(numFrame);
    if(dictionary_has_key(TablaFrames, sNumFrame)){
        dictionary_remove(TablaFrames, sNumFrame);
    }
    free(sNumFrame);

    char *msg = string_from_format("Borrando entrada en Tabla de Frames -> Frame: %u", numFrame);
    loggear_info(loggerAux, msg, true);
}