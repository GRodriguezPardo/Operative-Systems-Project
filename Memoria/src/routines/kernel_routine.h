#ifndef KERNELROUTINE_H_
#define KERNELROUTINE_H_

#include "../memoria_utils.h"
#include <stdlib.h>
#include <thesenate/tcp_serializacion.h>
#include "../paginacion.h"

void kernel_routine(int socketFd, int *returnStatus);

#endif /* KERNELROUTINE_H_ */