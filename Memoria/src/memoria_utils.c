#include "memoria_utils.h"

static uint32_t buscar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa);
static bool posicion_esta_asignada(t_bitarray *bitmap, int frame);
static void marcar_posicion_asignada(t_bitarray *bitmap, int marco);


void esperar_hilos(){
	pthread_mutex_init(&mx_main, NULL);
	pthread_mutex_lock(&mx_main);
	pthread_mutex_lock(&mx_main);
}

void recibir_handshake(int socketFd, op_code codigo) {
    char *msg = string_from_format("Se conectó %s.", (codigo == INIT_KERNEL) ? "Kernel" : "CPU");
    loggear_info(logger, msg);
    free(msg);

    int __attribute__((unused)) tamanio = largo_paquete(socketFd);
    void *handshakeMsg = recibir(socketFd);
    msg = string_from_format("ACK Recibido: %d", *(int *)handshakeMsg);
    loggear_info(logger, msg);
    free(handshakeMsg);
    free(msg);
}

void loggear_info(t_log *log, char *msg){
    pthread_mutex_lock(&mx_logger);
    log_info(log, msg);
    pthread_mutex_unlock(&mx_logger);
}

void loggear_error(t_log *log, char *msg){
    pthread_mutex_lock(&mx_logger);
    log_error(log, msg);
    pthread_mutex_unlock(&mx_logger);
}

void aplicar_retardo(uint32_t tiempo_ms){
    sleep(tiempo_ms / 1000); //tiempo_ms está en milisegundos
}

uint32_t recibir_uint32t(int socket){
    uint32_t valor;
    void *buffer = recibir(socket);
    valor = *((uint32_t *)buffer);
    free(buffer);
    return valor;
}

t_list *get_tablas_proceso(uint32_t _idProceso){
    t_list *lista;
    bool _esTablaDelProceso(void *item){
        t_tablaPaginas *tabla = (t_tablaPaginas *)item;
        return tabla->idProceso == _idProceso;
    }
    lista = list_filter(EspacioTablasPag, _esTablaDelProceso);
    return lista;
}

int sonIguales(const char* str1, const char* str2){
    return !strcmp(str1, str2);
}

uint32_t asignar_posicion_libre(t_bitarray *bitmap, uint32_t longitudMapa){
    uint32_t pos = buscar_posicion_libre(bitmap, longitudMapa);
    marcar_posicion_asignada(bitmap, pos);
    return pos;
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

static bool posicion_esta_asignada(t_bitarray *bitmap, int frame){
    return bitarray_test_bit(bitmap, frame);
}

static void marcar_posicion_asignada(t_bitarray *bitmap, int frame){
    bitarray_set_bit(bitmap, frame);
}