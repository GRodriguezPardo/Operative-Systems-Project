#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdint.h>

int iniciar_servidor(char *ip, char *puerto, void *(*start_routine)(void *));

int iniciar_servidor_consolas(char *ip, char *puerto, void *(*start_routine)(void *), int **thread_identificator, uint8_t **thread_status);

#endif /* TCP_SERVER_H_ */