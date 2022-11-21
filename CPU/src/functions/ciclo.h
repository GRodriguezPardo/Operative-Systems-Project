#ifndef CICLO_H_
#define CICLO_H_

#include "../globals.h"
#include "../cpu_utils.h"
#include "./mmu.h"
#include "./tlb.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct t_auxCiclo{
    char op[10], 
    oper1[10], 
    oper2[10];
    uint8_t instruction_code;
    uint32_t register1,register2;
}t_auxCiclo;

extern bool devolverContexto;

char* fetch();
void decode(t_log *logger_cpu_ciclo, long retardo_instruccion, char* instruccion, t_auxCiclo* auxCiclo);
void execute(t_auxCiclo* auxCiclo);
void check_interrupt();
void *ciclo_instruccion(void *config);



#endif