#ifndef TCP_SERIALIZACION_H_
#define TCP_SERIALIZACION_H_

typedef enum op_code
{
    // EJEMPLOS
    MENSAJE,
    RESPUESTA,

    // OPERACIONES
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

//emision
void crear_buffer(t_paquete*);
t_paquete* crear_paquete(op_code);
void agregar_a_paquete(t_paquete*, void*, int);
void enviar_paquete(t_paquete*, int);
void eliminar_paquete(t_paquete*);

//recepcion
void * recibir(int);
int recibir_operacion(int);
int largo_paquete(int);
void* recibir_buffer(int*, int);
void* serializar_paquete(t_paquete*, int);

#endif /* TCP_SERIALIZACION_H_ */