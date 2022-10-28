#include "swap.h"

void swap_inicializar() {
    /// Inicializacion espacio SWAP ///
    char *pathSwap = config_get_string_value(config, "PATH_SWAP");
    int tamanioSwap = config_get_int_value(config, "TAMANIO_SWAP");
    swapFd = open(pathSwap, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(swapFd, tamanioSwap);
}

void swap_cerrar() {
    close(swapFd);
}