#ifndef TCP_SERIALIZACION_H_
#define TCP_SERIALIZACION_H_

typedef enum op_code
{
    // EJEMPLOS
    MENSAJE,
    RESPUESTA,

    // INICIO DE CONSOLA
    NUEVO_PROCESO,

    // OPERACIONES I/O
    CONSOLE_INPUT,
    CONSOLE_INPUT_RESPUESTA,
    CONSOLE_OUTPUT,
    CONSOLE_OUTPUT_RESPUESTA,

    // OPERACIONES
    SET,
    ADD,
    MOV_IN,
    MOV_OUT,
    I_O,
    EXIT,

    // MEMORIA
    INIT_KERNEL,
    MMU_MARCO,
    PAGE_FAULT,
    READY_PROCESO,

    // KERNEL - CPU
    INIT_CPU,
    DESALOJO_PROCESO,
    EXIT_PROCESO,
    BLOQUEO_PROCESO,
    INTERRUPCION,
    PROXIMO_PCB,
    CONTEXTO,

    //MEMORIA - CPU
    INIT_MEMORIA,
    DIRECCION_FISICA
    
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
void crear_buffer(t_paquete *paquete);
t_paquete *crear_paquete(op_code op_code);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void eliminar_paquete(t_paquete *paquete);

//recepcion
void *recibir(int socket_cliente);
int recibir_operacion(int socket_cliente);
int largo_paquete(int socket_cliente);
void *recibir_buffer(int *size, int socket_cliente);
void *serializar_paquete(t_paquete *paquete, int bytes);

#endif /* TCP_SERIALIZACION_H_ */