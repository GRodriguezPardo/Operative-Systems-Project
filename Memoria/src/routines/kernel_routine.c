#include "kernel_routine.h"

void kernel_routine(int socketFd, int* returnStatus) {
    recibirHandshake(socketFd, KERNEL_ACK);
}