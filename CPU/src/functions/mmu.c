#include "mmu.h"
#include <math.h>

bool chequearPageFault(t_configMemoria* configMemoria, t_log* loggerMMU, uint32_t num_segmento, uint32_t num_pagina){
    if(configMemoria->pipelineMemoria.operacion == PAGE_FAULT){
            pthread_mutex_lock(&mutex_logger);
            log_info(loggerMMU,"Page Fault PID: %d - Segmento: %d - Pagina: %d",mi_contexto->id,num_segmento,num_pagina);
            pthread_mutex_unlock(&mutex_logger);   
            return true;
    } 
    return false;
}

uint32_t calcularDirecFisica(uint32_t marco, uint32_t desplazamiento_pag){
    return marco * (configMemoria->tamanioPagina) + desplazamiento_pag;
}

void buscarMarco(uint32_t nroSegmento, uint32_t idPagina){
    for(uint32_t i = 0; i < mi_contexto->cantSegmentos; i++){
        if(mi_contexto->segmentos[i].nro_segmento == nroSegmento){
            configMemoria->pipelineMemoria.operacion = MMU_MARCO;
            configMemoria->pipelineMemoria.idTablaPagina = mi_contexto->segmentos[i].identificador_tabla;
            configMemoria->pipelineMemoria.idPagina = idPagina;
        }
    }
}

/*
void reemplazoTLB(uint32_t num_segmento, uint32_t num_pagina, uint32_t num_marco){
    bool tlb_incompleta;
   
    for(uint i = 0; i < configMemoria->entradasTLB; i ++){
        if(!tlb[i].presente){
            // agregar_algoritmo();
            //indice = i;
            tlb_incompleta = true;
            //break;
        }
    }
    if(!tlb_incompleta){
        //reemplazar_algoritmo(indice);
    }
} */

op_code traducciones(op_code instruccion){ 
    uint32_t num_segmento, desplazamiento_segmento, num_pagina, desplazamiento_pag,
    direcFisica, direcLogica;
    bool pageFault;

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
    int num_marco = buscarEnTLB(mi_contexto->id,num_segmento,num_pagina);

    if(num_marco != -1){ 
        direcFisica = calcularDirecFisica(num_marco,desplazamiento_pag);
        configMemoria->pipelineMemoria.operacion = instruccion;  
        configMemoria->pipelineMemoria.direcFisica = direcFisica;
           
        pthread_mutex_lock(&mutex_logger);
        log_info(loggerMMU,"PID: %d - TLB HIT - Segmento: %d - Pagina: %d",mi_contexto->id, num_segmento, num_pagina);
        pthread_mutex_unlock(&mutex_logger);         

        sem_post(&sem_conexion_memoria);
        sem_wait(&sem_mmu);
        
        pageFault = chequearPageFault(configMemoria,loggerMMU,num_segmento,num_pagina);

        if(pageFault){
            return PAGE_FAULT;
        }

        return VALOR_OK;//configMemoria->pipelineMemoria.valor;
    }else{
        pthread_mutex_lock(&mutex_logger);
        log_info(loggerMMU,"PID: %d - TLB MISS - Segmento: %d - Pagina: %d",mi_contexto->id,num_segmento,num_pagina);
        pthread_mutex_unlock(&mutex_logger); 

        buscarMarco(num_segmento,num_pagina);
        
        sem_post(&sem_conexion_memoria);
        sem_wait(&sem_mmu);

        pageFault = chequearPageFault(configMemoria,loggerMMU,num_segmento,num_pagina);

        if(pageFault){
            return PAGE_FAULT;
        }

        agregar_entrada_tlb(mi_contexto->id, num_segmento, num_pagina, configMemoria->numMarco); 
  
        direcFisica = calcularDirecFisica(configMemoria->numMarco,desplazamiento_pag);
        configMemoria->pipelineMemoria.operacion = instruccion;  
        configMemoria->pipelineMemoria.direcFisica = direcFisica;

        sem_post(&sem_conexion_memoria);
        sem_wait(&sem_mmu);

        pageFault = chequearPageFault(configMemoria,loggerMMU,num_segmento,num_pagina);

        if(pageFault){
            return PAGE_FAULT;
        }
        return VALOR_OK;
    }

}
