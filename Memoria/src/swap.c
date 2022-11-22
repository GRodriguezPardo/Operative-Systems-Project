#include "swap.h"

int swapFile; //file descriptor
t_pagina * (*seleccionarVictima)(t_queue*);

void swap_inicializar() {
    /// Inicializacion espacio SWAP ///
    swapFile = open(ConfigMemoria.pathSwap, O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    ftruncate(swapFile, ConfigMemoria.tamanioSwap);

    if (sonIguales(ConfigMemoria.algoritmoReemplazo, "CLOCK"))
        seleccionarVictima = &seleccionarV_Clock;
    else
        seleccionarVictima = &seleccionarV_ClockMejorado;
}

uint32_t swap_crear_pagina(){
    uint32_t numPaginaSwap = asignar_slot_swap_libre();
    uint32_t posicionEnSwap = numPaginaSwap * ConfigMemoria.tamanioPagina;
    return posicionEnSwap;
}

uint32_t swap_resolver_pageFault(uint32_t pid, uint32_t idTabla, uint32_t numPagina){
    uint32_t frameAsignado;
    t_pagina *paginaReferida = get_pagina(pid, idTabla, numPagina);

    if(memoria_esta_llena() || proceso_alcanzo_max_marcos(pid)){
        frameAsignado = reemplazarPagina(pid, paginaReferida);
    } else {
        frameAsignado = swap_in(pid, paginaReferida, NULL);
        t_infoProceso *dataP = get_info_proceso(pid);
        queue_push(dataP->paginasPresentes, paginaReferida);
    }

    return frameAsignado;
}

uint32_t swap_in(uint32_t pid, t_pagina *pagina, uint32_t numMarco){
    pagina->marco = numMarco == NULL ? asignar_frame_libre() : numMarco;

    void *paginaEnSwap = swap_leer_pagina(swapFile, pagina->posicion_swap);
    int posicion = pagina->marco * ConfigMemoria.tamanioPagina;
    pthread_mutex_lock(&mx_espacioUsuario);
    memcpy(EspacioUsuario + posicion, paginaEnSwap, ConfigMemoria.tamanioPagina);
    pthread_mutex_unlock(&mx_espacioUsuario);
    free(paginaEnSwap);

    pagina->presente = true;

    char *strOK = string_from_format("SWAP IN -  PID: %d - Marco: %d - Page In: S%d|P%d", pid, pagina->marco, pagina->numeroSegmento, pagina->id);
    loggear_info(loggerMain, strOK, true);

    return pagina->marco;
}

void swap_out(uint32_t pid, t_pagina *pagina){
    int offset = pagina->marco * ConfigMemoria.tamanioPagina;
    pthread_mutex_lock(&mx_espacioUsuario);
    swap_escribir_pagina(swapFile, pagina->posicion_swap, EspacioUsuario + offset);
    pthread_mutex_unlock(&mx_espacioUsuario);

    char *strOK = string_from_format("SWAP OUT -  PID: %d - Marco: %d - Page In: S%d|P%d", pid, pagina->marco, pagina->numeroSegmento, pagina->id);
    loggear_info(loggerMain, strOK, true);
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

uint32_t reemplazarPagina(uint32_t pid, t_pagina *paginaReferida){
    uint32_t numFrame;
    t_queue *qPaginasPresentes = (get_info_proceso(pid))->paginasPresentes;

    t_pagina *pagVictima = seleccionarVictima(qPaginasPresentes);

    char *strOK = string_from_format("REEMPLAZO - PID: %d - Marco: %d - Page Out: S%d|P%d - Page In: S%d|P%d", pid, pagVictima->marco, pagVictima->numeroSegmento, pagVictima->id, paginaReferida->numeroSegmento, paginaReferida->id);
    loggear_info(loggerMain, strOK, true);

    if (pagVictima->modificado){
        swap_out(pid, pagVictima);
        pagVictima->modificado = false;
    }
    pagVictima->presente = false;
    pagVictima->usado = false;
    
    numFrame = swap_in(pid, paginaReferida, pagVictima->marco);
    queue_push(qPaginasPresentes, paginaReferida);

    return numFrame;
}

t_pagina *seleccionarV_Clock(t_queue *qPaginasPresentes){
    //uso un queue porque de esta forma puedo usar el tope de la cola como el puntero del clock,
    //cuando evaluo una pagina, la saco de la cola, y si tengo que devolverla, la ubico
    //al final de la cola, con el tope apuntando a la proxima pagina a evaluar

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
    
    return pagVictima;
}

t_pagina *seleccionarV_ClockMejorado(t_queue *qPaginasPresentes){
    t_pagina *pagVictima;
    bool paginaVictimaEncontrada = false;
    do
    {
        //PASO 1 (u = 0, m = 0)
        for (int p = 0; p < ConfigMemoria.cantidadMarcosMemoria; p++)
        {
            pagVictima = queue_pop(qPaginasPresentes);
            if (!pagVictima->usado && !pagVictima->modificado)
            {
                paginaVictimaEncontrada = true;
                break;
            } else {
                queue_push(qPaginasPresentes, pagVictima);
            }
        }
        
        if (!paginaVictimaEncontrada)
        {
            //PASO 2 (u = 0, m = 1)
            for (int p = 0; p < ConfigMemoria.cantidadMarcosMemoria; p++)
            {
                pagVictima = queue_pop(qPaginasPresentes);
                if (!pagVictima->usado && pagVictima->modificado)
                {
                    paginaVictimaEncontrada = true;
                    break;
                } else {
                    pagVictima->usado = false;
                    queue_push(qPaginasPresentes, pagVictima);
                }
            }       
        }
    } while (!paginaVictimaEncontrada);
    
    return pagVictima;
}

//auxiliares

bool memoria_esta_llena(){
    pthread_mutex_lock(&mx_tablaFrames);
    uint32_t cantidadMarcosUsados = (uint32_t)dictionary_size(TablaFrames);
    pthread_mutex_unlock(&mx_tablaFrames);

    return cantidadMarcosUsados == ConfigMemoria.cantidadMarcosMemoria;
}

bool proceso_alcanzo_max_marcos(uint32_t pid){
    t_infoProceso *dataP = get_info_proceso(pid);
    return queue_size(dataP->paginasPresentes) == ConfigMemoria.marcosPorProceso;
}