#include "swap.h"

int swapFile; //file descriptor
uint32_t (*reemplazarPagina)(uint32_t, t_pagina *);
int punteroClock = 0;

void swap_inicializar() {
    /// Inicializacion espacio SWAP ///
    swapFile = open(ConfigMemoria.pathSwap, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(swapFile, ConfigMemoria.tamanioSwap);

    if (sonIguales(ConfigMemoria.algoritmoReemplazo, "CLOCK"))
        reemplazarPagina = &reemplazar_Clock;
    else
        reemplazarPagina = &reemplazar_ClockM;
}

uint32_t swap_crear_pagina(){
    uint32_t numPaginaSwap = asignar_slot_swap_libre();
    uint32_t posicionEnSwap = numPaginaSwap * ConfigMemoria.tamanioPagina;
    return posicionEnSwap;
}

uint32_t swap_resolver_pageFault(uint32_t pid, uint32_t idTabla, uint32_t numPagina){
/*
- la memoria esta llena || el proceso tiene cargado su maximo de paginas
    false -> cargar la pagina en memoria (actualizar flags en tabla)
    true -> algoritmo de reemplazo
        - seleccionar la victima
        - pagina victima está modificada
            false -> se reemplaza directamente
            true -> - se hace el backup en swap
            se reemplaza
*/
    uint32_t frameAsignado;
    t_pagina *paginaReferida = pag_get_pagina(pid, idTabla, numPagina);

    if(memoria_esta_llena() || proceso_alcanzo_max_marcos(pid)){
        frameAsignado = reemplazarPagina(pid, paginaReferida);
    } else {
        frameAsignado = swap_in(paginaReferida, NULL);
        t_dataProceso *dataP = get_data_proceso(pid);
        queue_push(dataP->paginasPresentes, paginaReferida);
    }

    return frameAsignado;
}

uint32_t swap_in(t_pagina *pagina, uint32_t numMarco){
    pagina->marco = numMarco == NULL ? asignar_frame_libre() : numMarco;

    void *paginaEnSwap = swap_leer_pagina(swapFile, pagina->posicion_swap);
    int posicion = pagina->marco * ConfigMemoria.tamanioPagina;
    pthread_mutex_lock(&mx_espacioUsuario);
    memcpy(EspacioUsuario + posicion, paginaEnSwap, ConfigMemoria.tamanioPagina);
    pthread_mutex_unlock(&mx_espacioUsuario);
    free(paginaEnSwap);

    pagina->presente = true;

    return pagina->marco;
}

void swap_out(t_pagina *pagina){
    int offset = pagina->marco * ConfigMemoria.tamanioPagina;
    pthread_mutex_lock(&mx_espacioUsuario);
    swap_escribir_pagina(swapFile, pagina->posicion_swap, EspacioUsuario + offset);
    pthread_mutex_unlock(&mx_espacioUsuario);
}

void *swap_leer_pagina(int fd, uint32_t posicionSwap){
    void *buffer = malloc(ConfigMemoria.tamanioPagina);
    pread(fd, buffer, ConfigMemoria.tamanioPagina, posicionSwap);
    return buffer;
}

void swap_escribir_pagina(int fd, uint32_t posicionSwap, void *inicioPagina){
    pwrite(fd, inicioPagina, ConfigMemoria.tamanioPagina, posicionSwap);
    fsync(fd);
}

void swap_cerrar() {
    ftruncate(swapFile, 0);
    close(swapFile);
}


/* ALGORITMOS REEMPLAZO */

uint32_t reemplazar_Clock(uint32_t pid, t_pagina *paginaReferida){

    uint32_t numFrame;

    //uso un queue porque de esta forma puedo usar el tope de la cola como el puntero del clock,
    //cuando evaluo una pagina, la saco de la cola, y si tengo que devolverla, la ubico
    //al final de la cola, con el tope apuntando a la proxima pagina a evaluar

    t_queue *qPaginasPresentes = (get_data_proceso(pid))->paginasPresentes;

    //Elegir la victima
    t_pagina *pagVictima;
    while (true)
    {
        pagVictima = queue_pop(qPaginasPresentes);
        if (pagVictima->usado)
        {
            pagVictima->usado = false;
            queue_push(qPaginasPresentes, pagVictima);
        } else {
            break;
        }
    }
    
    //Reemplazar la victima

    if (pagVictima->modificado){
        swap_out(pagVictima);
        pagVictima->modificado = false;
    }
    pagVictima->presente = false;
    pagVictima->usado = false;
    
    numFrame = swap_in(paginaReferida, pagVictima->marco);
    queue_push(qPaginasPresentes, paginaReferida);

    //LOGUEAR
    return numFrame;
    
}

uint32_t reemplazar_ClockM(uint32_t pid, t_pagina *paginaReferida){
    /*
    uint32_t numFrame;

    t_list *lPaginasPresentes = (get_data_proceso(pid))->paginasPresentes;

    //Elegir la victima
    t_pagina *pagVictima;
    while (true)
    {
        pagVictima = list_get(lPaginasPresentes, punteroClock);
        if (!pagVictima->usado && !pagVictima->modificado)
        {
            break;
        } else if ()
        {
            pagVictima->usado = false;
            punteroClock = (punteroClock + 1) % ConfigMemoria.marcosPorProceso; //si llego al indice maximo, vuelvo a empezar
        }
        {
            break;
        }
    }
    
    //Reemplazar la victima
    //if (pagVictima->modificado)
        //swap_out();
        //pagVictima->modificado = false;
    
    pagVictima->presente = false;
    pagVictima->usado = false;
    list_remove(lPaginasPresentes, punteroClock); //remuevo la pagina victima
    
    numFrame = swap_in(paginaReferida, pagVictima->marco);
    list_add_in_index(lPaginasPresentes, punteroClock, paginaReferida);

    //LOGUEAR
    return numFrame;
    */
}

//STATIC

bool memoria_esta_llena(){
    pthread_mutex_lock(&mx_tablaFrames);
    uint32_t cantidadMarcosUsados = (uint32_t)dictionary_size(TablaFrames);
    pthread_mutex_unlock(&mx_tablaFrames);

    return cantidadMarcosUsados == ConfigMemoria.cantidadMarcosMemoria;
}

bool proceso_alcanzo_max_marcos(uint32_t pid){
    t_dataProceso *dataP = get_data_proceso(pid);
    return queue_size(dataP->paginasPresentes) == ConfigMemoria.marcosPorProceso;
}