#include "./tlb.h"

void iniciar_estructuras(){
    colaTLB = list_create();
}

void agregar_entrada_tlb(uint32_t pid, uint32_t num_segmento, uint32_t num_pagina, uint32_t num_marco){
    t_tlb *tlb = malloc(sizeof(t_tlb));
    tlb->pid = pid;
    tlb->nro_segmento = num_segmento;
    tlb->nro_pag = num_pagina;
    tlb->marco = num_marco;

    if(list_size(colaTLB) == configMemoria->entradasTLB)
    {
        reemplazar_pagina(tlb);
        return;
    }

    list_add(colaTLB, tlb);
}

int buscarEnTLB(uint32_t pid, uint32_t num_segmento, uint32_t num_pagina)
{
    for(int i = 0; i < list_size(colaTLB); i++)
    {
        t_tlb *info = list_get(colaTLB, i);

        if(info->pid == pid && info->nro_pag == num_pagina && info->nro_segmento == num_segmento)
        {
            if(strcmp(reemplazo_tlb, "LRU") == 0)
            {
                t_tlb *entrada = list_remove(colaTLB, i);
                list_add(colaTLB, entrada);
            }
            return info->marco;
        }
    }

    return -1;
}

/*
    NOTA: los criterios son LRU y FIFO
    -En el caso de FIFO solo se elimina el primer elemento (el primero que fue agregado)
    y se agrega el nuevo al final (ultimo en llegar)
    -En el caso de LRU se reordena la lista cada vez que hay un TLB HIT
    (se referencia una entrada que esta en la tlb) por lo que ambas funciones
    reemplazan la pagina de la misma forma
*/
void reemplazar_pagina(t_tlb *entrada)
{
 //   t_tlb *aux = list_get(colaTLB, 0);
    list_remove_and_destroy_element(colaTLB, 0, free);
    list_add(colaTLB, entrada);
}

void limpiar_tlb(){
    list_remove_and_destroy_all_by_condition(colaTLB,condicionPID,free);   
}

bool condicionPID(void *tlb){
    t_tlb aux = *(t_tlb*)tlb;
    return aux.pid == mi_contexto->id;
}