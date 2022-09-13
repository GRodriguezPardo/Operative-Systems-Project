
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>

//emision

typedef enum
{
    SET,
    ADD,
    MOV_IN,
    MOV_OUT,
    I_O,
    EXIT
}op_code;

typedef struct
{
    int size;
    void* stream;
} t_buffer;

typedef struct
{
    op_code codigo_operacion;
    t_buffer* buffer;
} t_paquete;

void crear_buffer(t_paquete* paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = 0;
    paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(op_code op_code){
    t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = op_code;
    crear_buffer(paquete);
    return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

    memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

    paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
    int bytes = paquete->buffer->size + 2*sizeof(int);
    void* a_enviar = serializar_paquete(paquete, bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}
//recepcion
void * recibir(int socket_cliente){

    void * valor;
    int tamanio;
    recv(socket_cliente, &tamanio, sizeof(int), MSG_WAITALL);

    valor = malloc(tamanio);
    recv(socket_cliente,valor, tamanio, MSG_WAITALL);


    return valor;


}

int recibir_operacion(int socket_cliente)
{
    int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

int largo_paquete(int socket_cliente){
    int largo;

    recv(socket_cliente, &largo, sizeof(int), MSG_WAITALL);

    return largo;

}

void* recibir_buffer(int* size, int socket_cliente)
{
    void * buffer;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);

    return buffer;
}