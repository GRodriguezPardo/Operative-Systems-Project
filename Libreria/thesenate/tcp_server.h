#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

int iniciar_servidor(char*, char*, void *(*start_routine)(void *));

#endif /* TCP_SERVER_H_ */