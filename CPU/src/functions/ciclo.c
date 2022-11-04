#include "ciclo.h"

void* ciclo_instruccion(void* config){
    
    t_log *logger_cpu_ciclo = log_create("../cpu.log", "CPU - Ciclo_instruccion", 0, LOG_LEVEL_INFO);
    long retardo_instruccion = config_get_long_value((t_config*) config,"RETARDO_INSTRUCCION");
    char op[10], 
    oper1[10], 
    oper2[10];
    char* instruccion;
    uint8_t instruction_code;
    uint8_t register1,register2;
    bool devolverContexto;
    ////////////// FETCH //////////////
    sem_wait(&sem_ciclo_instruccion);
    pthread_mutex_lock(&mutex_logger);
    log_info(logger_cpu_ciclo,"%s",mi_contexto->instrucciones[mi_contexto->program_counter]);
    pthread_mutex_unlock(&mutex_logger);
    while(1){
            {
            instruccion = mi_contexto->instrucciones[mi_contexto->program_counter];
            }
        ////////////// DECODE //////////////
            pthread_mutex_lock(&mutex_logger);
            log_info(logger_cpu_ciclo,"%s",instruccion);
            pthread_mutex_unlock(&mutex_logger);
            {
                sscanf(instruccion, "%s %s %s", op, oper1, oper2);
                pthread_mutex_lock(&mutex_logger);
                log_info(logger_cpu_ciclo,"la operacion es :%s",op);
                pthread_mutex_unlock(&mutex_logger);
                if(!strcmp(op,"SET")){
                    if(!strcmp(oper1,"AX")){
                        register1 = 0;
                    } else if(!strcmp(oper1,"BX")){
                        register1 = 1;
                    }else if(!strcmp(oper1,"CX")){
                        register1 = 2;
                    }else if(!strcmp(oper1,"DX")){
                        register1 = 3;
                    }        
                    usleep(retardo_instruccion * 1000);
                    instruction_code = 0;
                }else if(!strcmp(op,"ADD")){
                    if(!strcmp(oper1,"AX")){
                        register1 = 0;
                    } else if(!strcmp(oper1,"BX")){
                        register1 = 1;
                    }else if(!strcmp(oper1,"CX")){
                        register1 = 2;
                    }else if(!strcmp(oper1,"DX")){
                        register1 = 3;    
                    }
                    if(!strcmp(oper2,"AX")){
                        register2 = 0;
                    } else if(!strcmp(oper2,"BX")){
                        register2 = 1;
                    }else if(!strcmp(oper2,"CX")){
                        register2 = 2;
                    }else if(!strcmp(oper2,"DX")){
                        register2 = 3;
                    }
                    usleep(retardo_instruccion * 1000);
                    instruction_code = 1;   
                }else if(!strcmp(op,"MOV_IN")){
                    instruction_code = 2;
                }else if(!strcmp(op,"MOV_OUT")){
                    instruction_code = 3;
                }else if(!strcmp(op,"I/O")){
                    instruction_code = 4;
                }else if(!strcmp(op,"EXIT")){
                    instruction_code = 5;
                }
            }
            ////////////// EXECUTE //////////////
            {
                switch(instruction_code){
                    case 0://set
                        mi_contexto->registros[register1] = (uint32_t)atoi(oper2);
                        break;
                    case 1://add
                        mi_contexto->registros[register1] += mi_contexto->registros[register2];
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4://I/O
                        mi_contexto->dispositivo = oper1;
                        mi_contexto->unidades = (uint32_t)atoi(oper2);
                        devolverContexto = true;
                        mi_contexto -> pipeline.operacion = BLOQUEO_PROCESO; //pensar prioridades de razones para desalojar
                        break;
                    case 5://EXIT
                        devolverContexto = true;
                        mi_contexto -> pipeline.operacion = EXIT_PROCESO;
                        break;
                    default:
                }
            }
        mi_contexto->program_counter++;
        pthread_mutex_lock(&mutex_logger);
        log_info(logger_cpu_ciclo,"PID:%d - Ejecutando: %s - %s - %s",mi_contexto->id,op,oper1,oper2);
        pthread_mutex_unlock(&mutex_logger);
        ////////////// CHECK INTERRUPT //////////////
        {
            if(flag_interrupcion == 1){
                if(pid_interrupt == mi_contexto->id){
                    if(mi_contexto->pipeline.operacion == PROXIMO_PCB){
                        pthread_mutex_lock(&mutex_flag);
                        flag_interrupcion = 0;
                        pthread_mutex_unlock(&mutex_flag);
                        devolverContexto = true;
                        mi_contexto -> pipeline.operacion = DESALOJO_PROCESO;
                    }
                }
            }
        }
        if(devolverContexto){
            devolverContexto = false;
            sem_post(&sem_envio_contexto);
            sem_wait(&sem_ciclo_instruccion);
        }
        
    }
    
}