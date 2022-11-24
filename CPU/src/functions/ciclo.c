#include "ciclo.h"

bool devolverContexto;

char *fetch()
{
    return mi_contexto->instrucciones[mi_contexto->program_counter];
}

void decode(t_log *logger_cpu_ciclo, long retardo_instruccion, char *instruccion, t_auxCiclo *auxCiclo)
{
    sscanf(instruccion, "%s %s %s", auxCiclo->op, auxCiclo->oper1, auxCiclo->oper2);
    pthread_mutex_lock(&mutex_logger);
    log_info(logger_cpu_ciclo, "la operacion es :%s", auxCiclo->op);
    pthread_mutex_unlock(&mutex_logger);
    if (!strcmp(auxCiclo->op, "SET"))
    {
        if (!strcmp(auxCiclo->oper1, "AX"))
        {
            auxCiclo->register1 = 0;
        }
        else if (!strcmp(auxCiclo->oper1, "BX"))
        {
            auxCiclo->register1 = 1;
        }
        else if (!strcmp(auxCiclo->oper1, "CX"))
        {
            auxCiclo->register1 = 2;
        }
        else if (!strcmp(auxCiclo->oper1, "DX"))
        {
            auxCiclo->register1 = 3;
        }
        usleep(retardo_instruccion * 1000);
        auxCiclo->instruction_code = 0;
    }
    else if (!strcmp(auxCiclo->op, "ADD"))
    {
        if (!strcmp(auxCiclo->oper1, "AX"))
        {
            auxCiclo->register1 = 0;
        }
        else if (!strcmp(auxCiclo->oper1, "BX"))
        {
            auxCiclo->register1 = 1;
        }
        else if (!strcmp(auxCiclo->oper1, "CX"))
        {
            auxCiclo->register1 = 2;
        }
        else if (!strcmp(auxCiclo->oper1, "DX"))
        {
            auxCiclo->register1 = 3;
        }
        if (!strcmp(auxCiclo->oper2, "AX"))
        {
            auxCiclo->register2 = 0;
        }
        else if (!strcmp(auxCiclo->oper2, "BX"))
        {
            auxCiclo->register2 = 1;
        }
        else if (!strcmp(auxCiclo->oper2, "CX"))
        {
            auxCiclo->register2 = 2;
        }
        else if (!strcmp(auxCiclo->oper2, "DX"))
        {
            auxCiclo->register2 = 3;
        }
        usleep(retardo_instruccion * 1000);
        auxCiclo->instruction_code = 1;
    }
    else if (!strcmp(auxCiclo->op, "MOV_IN"))
    {
        if (!strcmp(auxCiclo->oper1, "AX"))
        {
            auxCiclo->register1 = 0;
        }
        else if (!strcmp(auxCiclo->oper1, "BX"))
        {
            auxCiclo->register1 = 1;
        }
        else if (!strcmp(auxCiclo->oper1, "CX"))
        {
            auxCiclo->register1 = 2;
        }
        else if (!strcmp(auxCiclo->oper1, "DX"))
        {
            auxCiclo->register1 = 3;
        }
        auxCiclo->instruction_code = 2;
    }
    else if (!strcmp(auxCiclo->op, "MOV_OUT"))
    {
        if (!strcmp(auxCiclo->oper2, "AX"))
        {
            auxCiclo->register2 = 0;
        }
        else if (!strcmp(auxCiclo->oper2, "BX"))
        {
            auxCiclo->register2 = 1;
        }
        else if (!strcmp(auxCiclo->oper2, "CX"))
        {
            auxCiclo->register2 = 2;
        }
        else if (!strcmp(auxCiclo->oper2, "DX"))
        {
            auxCiclo->register2 = 3;
        }
        auxCiclo->instruction_code = 3;
    }
    else if (!strcmp(auxCiclo->op, "I/O"))
    {
        auxCiclo->instruction_code = 4;
    }
    else if (!strcmp(auxCiclo->op, "EXIT"))
    {
        auxCiclo->instruction_code = 5;
    }
}

void execute(t_auxCiclo *auxCiclo)
{
    op_code operacion;
    switch (auxCiclo->instruction_code)
    {
    case 0: // set
        mi_contexto->registros[auxCiclo->register1] = (uint32_t)atoi(auxCiclo->oper2);
        break;
    case 1: // add
        mi_contexto->registros[auxCiclo->register1] += mi_contexto->registros[auxCiclo->register2];
        break;
    case 2: // mov_in
        operacion = MOV_IN;
        configMemoria->pipelineMemoria.direcLogica = (uint32_t)atoi(auxCiclo->oper2);
        operacion = traducciones(operacion);
        switch (operacion)
        {
        case SEG_FAULT:
            flag_segFault = 1;
            mi_contexto->pipeline.operacion = SEG_FAULT;
            limpiar_tlb();
            break;
        case PAGE_FAULT:
            flag_pageFault = 1;
            mi_contexto->pipeline.operacion = PAGE_FAULT;
            break;
        case VALOR_OK:
            mi_contexto->registros[auxCiclo->register1] = configMemoria->pipelineMemoria.valor;
            break;
        default:
        }
        // ver si hace falta pisar los valores del pipeline
        break;
    case 3: // mov_out
        operacion = MOV_OUT;
        configMemoria->pipelineMemoria.direcLogica = (uint32_t)atoi(auxCiclo->oper1);
        configMemoria->pipelineMemoria.valor = mi_contexto->registros[auxCiclo->register2];
        operacion = traducciones(operacion);
        switch (operacion)
        {
        case SEG_FAULT:
            flag_segFault = 1;
            mi_contexto->pipeline.operacion = SEG_FAULT;
            limpiar_tlb();
            break;
        case PAGE_FAULT:
            flag_pageFault = 1;
            mi_contexto->pipeline.operacion = PAGE_FAULT;
            break;
        case VALOR_OK:
            break;
        default:
        }
        // ver si hace falta pisar los valores del pipeline
        break;
    case 4: // I/O
        mi_contexto->dispositivo = auxCiclo->oper1;
        if (!strcmp(mi_contexto->dispositivo, "TECLADO") || !strcmp(mi_contexto->dispositivo, "PANTALLA"))
        {
            if (!strcmp(auxCiclo->oper2, "AX"))
            {
                mi_contexto->unidades = 0;
            }
            else if (!strcmp(auxCiclo->oper2, "BX"))
            {
                mi_contexto->unidades = 1;
            }
            else if (!strcmp(auxCiclo->oper2, "CX"))
            {
                mi_contexto->unidades = 2;
            }
            else if (!strcmp(auxCiclo->oper2, "DX"))
            {
                mi_contexto->unidades = 3;
            }
        }
        else
        {
            mi_contexto->unidades = (uint32_t)atoi(auxCiclo->oper2);
        }

        devolverContexto = true;
        mi_contexto->pipeline.operacion = BLOQUEO_PROCESO; // pensar prioridades de razones para desalojar
        break;
    case 5: // EXIT
        devolverContexto = true;
        mi_contexto->pipeline.operacion = EXIT_PROCESO;
        limpiar_tlb();
        break;
    default:
    }
}

void check_interrupt()
{
    pthread_mutex_lock(&mutex_flag);
    if (flag_interrupcion == 1)
    {
        if (pid_interrupt == mi_contexto->id)
        {
            if (mi_contexto->pipeline.operacion == PROXIMO_PCB)
            { // cuidado cuando tocamos la pipeline.operacion para mandar msg a memoria

                flag_interrupcion = 0;

                devolverContexto = true;
                mi_contexto->pipeline.operacion = DESALOJO_PROCESO;
            }
        }
        else
        {
            flag_interrupcion = 0;
        }
    }
    pthread_mutex_unlock(&mutex_flag);
}

void *ciclo_instruccion(void *config)
{

    t_log *logger_cpu_ciclo = log_create("../cpu.log", "CPU - Ciclo_instruccion", 0, LOG_LEVEL_INFO);
    long retardo_instruccion = config_get_long_value((t_config *)config, "RETARDO_INSTRUCCION");

    char *instruccion;
    t_auxCiclo *auxCiclo = (t_auxCiclo *)malloc(sizeof(t_auxCiclo));

    sem_wait(&sem_ciclo_instruccion);

    while (1)
    {
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_cpu_ciclo, "%s", mi_contexto->instrucciones[mi_contexto->program_counter]);
        pthread_mutex_unlock(&mutex_logger);
        ////////////// FETCH //////////////
        {
            instruccion = fetch();
        }
        ////////////// DECODE //////////////
        {
            decode(logger_cpu_ciclo, retardo_instruccion, instruccion, auxCiclo);
        }
        ////////////// EXECUTE //////////////
        {
            execute(auxCiclo);
            // chequear que no hubo seg o page fault
            // si hubo mando las cosas con el op_code
        }
        if (flag_segFault != 1 && flag_pageFault != 1)
        {
            mi_contexto->program_counter++;

            pthread_mutex_lock(&mutex_logger);
            log_info(logger_cpu_ciclo, "PID:%d - Ejecutando: %s - %s - %s", mi_contexto->id, auxCiclo->op, auxCiclo->oper1, auxCiclo->oper2);
            pthread_mutex_unlock(&mutex_logger);

            ////////////// CHECK INTERRUPT //////////////
            {
                check_interrupt();
            }
            if (devolverContexto)
            {
                devolverContexto = false;
                sem_post(&sem_envio_contexto);
                sem_wait(&sem_ciclo_instruccion);
            }
        }
        else
        {
            flag_pageFault = 0;
            flag_segFault = 0;
            sem_post(&sem_envio_contexto);
            sem_wait(&sem_ciclo_instruccion);
        }
    }
}