#include "swap.h"

int swapFd;

void swap_inicializar() {
    /// Inicializacion espacio SWAP ///
    swapFd = open(configMemoria.pathSwap, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(swapFd, configMemoria.tamanioSwap);
}

void swap_cerrar() {
    close(swapFd);
}

/*
- el proceso tiene cargado su maximo de paginas?
    NO -> cargar la pagina en memoria (actualizar flags en tabla)
    SI -> algoritmo de reemplazo
        - seleccionar la victima
        - pagina victima está modificada?
            NO -> se reemplaza directamente
            SI -> - se hace el backup en swap
            se reemplaza
*/