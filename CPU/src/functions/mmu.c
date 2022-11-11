#include "mmu.h"
#include <math.h>



bool buscarEnTLB(uint32_t num_segmento, uint32_t num_pagina, uint32_t desplazamiento_pag){
    uint32_t direcFisica;
    bool encontrado = false;

    for(uint i = 0; i < configMemoria->entradasTLB; i++){
        if(tlb[i].pid == mi_contexto->id && tlb[i].nro_pag == num_pagina && tlb[i].nro_segmento == num_segmento){
           encontrado = true;
           direcFisica = calcularDirecFisica(tlb[i].marco,desplazamiento_pag);
           tlb[i].pipelineMemoria.operacion = DIRECCION_FISICA;               //revisar si no va ->!!!!
           tlb[i].pipelineMemoria.valor = direcFisica;
           sem_post(&sem_conexion_memoria);
           break;
        }
    }
    return encontrado;
}


uint32_t traducciones(uint32_t direcLogica){
    uint32_t num_segmento, desplazamiento_segmento, num_pagina, desplazamiento_pag;

    num_segmento = floor(direcLogica / configMemoria->tamanioMaximoSegmento);
    desplazamiento_segmento = direcLogica % configMemoria->tamanioMaximoSegmento;
    num_pagina = floor(desplazamiento_segmento  / configMemoria->tamanioPagina);
    desplazamiento_pag = desplazamiento_segmento % configMemoria->tamanioPagina;

    //chequear que el desplazamiento_segmento sea menor que el tam max del segmento
    //si no se cumple mandar a kernel y seg fault

    //busco si tengo guardado en la tlb el marco
    bool encontroTLB = buscarEnTLB(num_segmento,num_pagina,desplazamiento_pag);

    if(encontroTLB){
        //sem_wait(&sem_mmu);
    }else{
        //buscarMarco()
        //sem_wait(&sem_mmu);
        //reemplazoTLB(); ---> vamos a setear todos los pid en -1 cada vez que una entrada del tlb
        //                     esta vacia, si encontramos un -1 lo guardamos ahi sin hacer el algoritmo 
        //calcularDirFisica();
        //enviarInstruccion();
    }

    //si no esta, pedirle a la memoria el marco y ver si tira o no page fault
    //si tira page fault devolver a kernel y no actualizar el pc
    //si no guardar en la tlb o reemplazar 

}



uint32_t calcularDirecFisica(uint32_t marco, uint32_t desplazamiento_pag){
    return marco + desplazamiento_pag;
}



