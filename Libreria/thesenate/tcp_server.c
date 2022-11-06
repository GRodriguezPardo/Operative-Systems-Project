#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h>   //for threading , link with lpthread
#include "tcp_server.h"

int iniciar_servidor(char *ip, char *puerto, void *(*start_routine)(void *))
{
    int socket_servidor, client_sock, c;
    struct sockaddr_in server, client;

    // Create socket
    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor == -1)
    {
        perror("Could not create socket\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEPORT) failed");
        exit(EXIT_FAILURE);
    }
    puts("Socket created\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons((u_short)strtoul(puerto, NULL, 0));

    // Bind
    if (bind(socket_servidor, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        // print the error message
        perror("Bind failed. Error.");
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    // Listen
    listen(socket_servidor, SOMAXCONN);

    // Accept and incoming connection
    puts("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;

    while (1)
    {
        (client_sock = accept(socket_servidor, (struct sockaddr *)&client, (socklen_t *)&c));
        puts("Connection accepted\n");

        if (client_sock < 0)
        {
            perror("Accept failed\n");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&thread_id, NULL, start_routine, (void *)&client_sock) < 0)
        {
            perror("Could not create thread\n");
            exit(EXIT_FAILURE);
        }

        // Now join the thread , so that we dont terminate before the thread
        puts("Handler assigned\n");
    }

    return 0;
}

int iniciar_servidor_consolas(char *ip, char *puerto, void *(*start_routine)(void *), int **thread_identificator, uint8_t **thread_status)
{
    *thread_identificator = (int*)calloc(1, sizeof(int));
    *thread_status = (uint8_t*)calloc(1,sizeof(uint8_t));

    int socket_servidor, client_sock, c;
    struct sockaddr_in server, client;

    // Create socket
    socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_servidor == -1)
    {
        perror("Could not create socket\n");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0)
    {
        perror("setsockopt(SO_REUSEPORT) failed");
        exit(EXIT_FAILURE);
    }
    puts("Socket created\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_port = htons((u_short)strtoul(puerto, NULL, 0));

    // Bind
    if (bind(socket_servidor, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        // print the error message
        perror("Bind failed. Error.");
        exit(EXIT_FAILURE);
    }
    puts("Bind done");

    // Listen
    listen(socket_servidor, SOMAXCONN);

    // Accept and incoming connection
    puts("Waiting for incoming connections...\n");
    c = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    int* aux = (int*)calloc(2, sizeof(int));
    void* param = aux;

    while (1)
    {
        (client_sock = accept(socket_servidor, (struct sockaddr *)&client, (socklen_t *)&c));
        puts("Connection accepted\n");

        if (client_sock < 0)
        {
            perror("Accept failed\n");
            exit(EXIT_FAILURE);
        }

        aux[0] = client_sock;
        aux[1] =  **thread_identificator;

        *thread_status = (uint8_t*)realloc(*thread_status, sizeof(uint8_t)*(**thread_identificator + 1));
        (*thread_status)[**thread_identificator] = 1;

        **thread_identificator += 1;

        if (pthread_create(&thread_id, NULL, start_routine, param) < 0)
        {
            perror("Could not create thread\n");
            exit(EXIT_FAILURE);
        }

        // Now join the thread , so that we dont terminate before the thread
        puts("Handler assigned\n");
    }

    return 0;
}