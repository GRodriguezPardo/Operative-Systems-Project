#include "swap.h"

int swapFd;

void swap_inicializar() {
    /// Inicializacion espacio SWAP ///
    swapFd = open(configMemoria.path_swap, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(swapFd, configMemoria.tamanio_swap);
}

void swap_cerrar() {
    close(swapFd);
}