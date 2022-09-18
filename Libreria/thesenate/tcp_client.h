#ifndef TCP_CLIENT_H_
#define TCP_CLIENT_H_

int crear_conexion(char *ip, char *puerto);
void liberar_conexion(int socket_cliente);


#endif /* TCP_CLIENT_H_ */