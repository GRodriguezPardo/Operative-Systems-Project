#include "mmu.h"
#include <math.h>



bool buscarEnTLB(uint32_t num_segmento, uint32_t num_pagina, uint32_t desplazamiento_pag, op_code instruccion){
    uint32_t direcFisica;
    bool encontrado = false;

    for(uint i = 0; i < configMemoria->entradasTLB; i++){
        if(tlb[i].pid == mi_contexto->id && tlb[i].nro_pag == num_pagina && tlb[i].nro_segmento == num_segmento){
           encontrado = true;
           direcFisica = calcularDirecFisica(tlb[i].marco,desplazamiento_pag);
           configMemoria->pipelineMemoria.operacion = instruccion;  
           configMemoria->pipelineMemoria.direcFisica = direcFisica;
           sem_post(&sem_conexion_memoria);
           break;
        }
    }
    return encontrado;
}


uint32_t calcularDirecFisica(uint32_t marco, uint32_t desplazamiento_pag){
    return marco + desplazamiento_pag;
}

void buscarMarco(uint32_t nroSegmento, uint32_t idPagina){
    for(uint32_t i = 0; i < mi_contexto->cantSegmentos; i++){
        if(mi_contexto->segmentos[i].nro_segmento == nroSegmento){
            configMemoria->pipelineMemoria.operacion = MMU_MARCO;
            configMemoria->pipelineMemoria.idTablaPagina = mi_contexto->segmentos[i].identificador_tabla;
            configMemoria->pipelineMemoria.idPagina = idPagina;
            sem_post(&sem_conexion_memoria);
        }
    }
}

op_code traducciones(op_code instruccion){ 
    uint32_t num_segmento, desplazamiento_segmento, num_pagina, desplazamiento_pag,
    direcFisica, direcLogica;

    direcLogica = configMemoria->pipelineMemoria.direcLogica;


    num_segmento = floor(direcLogica / configMemoria->tamanioMaximoSegmento);
    desplazamiento_segmento = direcLogica % configMemoria->tamanioMaximoSegmento;
    num_pagina = floor(desplazamiento_segmento  / configMemoria->tamanioPagina);
    desplazamiento_pag = desplazamiento_segmento % configMemoria->tamanioPagina;

    //chequear que el desplazamiento_segmento sea menor que el tam max del segmento
    //si no se cumple mandar a kernel y seg fault
    if(desplazamiento_segmento > configMemoria->tamanioMaximoSegmento){
        return SEG_FAULT; 
    }

    //busco si tengo guardado en la tlb el marco
    bool encontroTLB = buscarEnTLB(num_segmento,num_pagina,desplazamiento_pag,instruccion);

    if(encontroTLB){ // REVISAR SI ES NECESARIO
        sem_wait(&sem_mmu);
        if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            return PAGE_FAULT;
        }
        return VALOR_OK;//configMemoria->pipelineMemoria.valor;
    }else{
        buscarMarco(num_segmento,num_pagina);
        sem_wait(&sem_mmu);
        if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            return PAGE_FAULT;
        }
        //reemplazoTLB(); ---> vamos a setear todos los pid en -1 cada vez que una entrada del tlb
        //                     esta vacia, si encontramos un -1 lo guardamos ahi sin hacer el algoritmo 
        direcFisica = calcularDirecFisica(configMemoria->numMarco,desplazamiento_pag);
        configMemoria->pipelineMemoria.operacion = instruccion;  
        configMemoria->pipelineMemoria.direcFisica = direcFisica;
        sem_wait(&sem_conexion_memoria);
        if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            return PAGE_FAULT;
        }
        return VALOR_OK;
    }

    //si no esta, pedirle a la memoria el marco y ver si tira o no page fault
    //si tira page fault devolver a kernel y no actualizar el pc
    //si no guardar en la tlb o reemplazar 

}



