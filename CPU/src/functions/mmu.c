#include "mmu.h"
#include <math.h>

uint32_t calcularDirecFisica(uint32_t marco, uint32_t desplazamiento_pag){
    return marco + desplazamiento_pag;
}

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

void reemplazoTLB(uint32_t num_segmento, uint32_t num_pagina, uint32_t num_marco){
    bool tlb_incompleta;
   
    for(uint i = 0; i < configMemoria->entradasTLB; i ++){
        if(tlb[i].pid == -1){
            // agregar_algoritmo();
            tlb_incompleta = true;
            //break;
        }
    }
    if(!tlb_incompleta){
        //reemplazar_algoritmo();
    }
}

op_code traducciones(op_code instruccion){ 
    uint32_t num_segmento, desplazamiento_segmento, num_pagina, desplazamiento_pag,
    direcFisica, direcLogica;

    t_log* loggerMMU;
    {
        char buffer[25];
        sprintf(buffer, "Cpu - MMU");
        loggerMMU = log_create("../cpu.log", buffer, 0, LOG_LEVEL_INFO);
    }


    direcLogica = configMemoria->pipelineMemoria.direcLogica;


    num_segmento = floor(direcLogica / configMemoria->tamanioMaximoSegmento);
    desplazamiento_segmento = direcLogica % configMemoria->tamanioMaximoSegmento;
    num_pagina = floor(desplazamiento_segmento  / configMemoria->tamanioPagina);
    desplazamiento_pag = desplazamiento_segmento % configMemoria->tamanioPagina;

    //chequear que el desplazamiento_segmento sea menor que el tam max del segmento
    //si no se cumple mandar a kernel y seg fault

    if(num_segmento >= mi_contexto->cantSegmentos || desplazamiento_segmento > mi_contexto->segmentos[num_segmento].tamanio){
        return SEG_FAULT; 
    }

    configMemoria->numSegActual = num_segmento;
    configMemoria->numPagActual = num_pagina;

    //busco si tengo guardado en la tlb el marco
    bool encontroTLB = buscarEnTLB(num_segmento,num_pagina,desplazamiento_pag,instruccion);

    if(encontroTLB){ // REVISAR SI ES NECESARIO
        pthread_mutex_lock(&mutex_logger);
        log_info(loggerMMU,"PID: %d - TLB HIT - Segmento: %d - Pagina: %d",mi_contexto->id, num_segmento, num_pagina);
        pthread_mutex_unlock(&mutex_logger);         

        sem_post(&sem_conexion_memoria);
        sem_wait(&sem_mmu);
        
                                                                        //TO-DO hacer funcion de page fault para no repetir codigo!!!

        if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            pthread_mutex_lock(&mutex_logger);
            log_info(loggerMMU,"Page Fault PID: %d - Segmento: %d - Pagina: %d",mi_contexto->id,num_segmento,num_pagina);
            pthread_mutex_unlock(&mutex_logger);   
            return PAGE_FAULT;
        }

        return VALOR_OK;//configMemoria->pipelineMemoria.valor;
    }else{
        pthread_mutex_lock(&mutex_logger);
        log_info(loggerMMU,"PID: %d - TLB MISS - Segmento: %d - Pagina: %d",mi_contexto->id,num_segmento,num_pagina);
        pthread_mutex_unlock(&mutex_logger); 

        buscarMarco(num_segmento,num_pagina);

        sem_wait(&sem_mmu);

        if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            pthread_mutex_lock(&mutex_logger);
            log_info(loggerMMU,"Page Fault PID: %d - Segmento: %d - Pagina: %d",mi_contexto->id,num_segmento,num_pagina);
            pthread_mutex_unlock(&mutex_logger);  
            return PAGE_FAULT;
        }

        reemplazoTLB(num_segmento, num_pagina, configMemoria->numMarco); 
                
        for(uint i = 0; i < configMemoria->entradasTLB; i ++){
            pthread_mutex_lock(&mutex_logger);
            log_info(loggerMMU,"%d |PID: %d |SEGMENTO: %d |PAGINA: %d |MARCO: %d",i,tlb[i].pid,tlb[i].nro_segmento,tlb[i].nro_pag,tlb[i].marco);
            pthread_mutex_unlock(&mutex_logger);
        }
        
        direcFisica = calcularDirecFisica(configMemoria->numMarco,desplazamiento_pag);
        configMemoria->pipelineMemoria.operacion = instruccion;  
        configMemoria->pipelineMemoria.direcFisica = direcFisica;

        sem_post(&sem_conexion_memoria);
        sem_wait(&sem_mmu);

        if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            pthread_mutex_lock(&mutex_logger);
            log_info(loggerMMU,"Page Fault PID: %d - Segmento: %d - Pagina: %d",mi_contexto->id,num_segmento,num_pagina);
            pthread_mutex_unlock(&mutex_logger);  
            return PAGE_FAULT;
        }
        return VALOR_OK;
    }

    

    //si no esta, pedirle a la memoria el marco y ver si tira o no page fault
    //si tira page fault devolver a kernel y no actualizar el pc
    //si no guardar en la tlb o reemplazar 

}
