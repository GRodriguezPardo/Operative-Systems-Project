#ifndef MEMORIA_ROUTINE_H_
#define MEMORIA_ROUTINE_H_

#include <stdint.h>
#include <stdio.h>
#include <thesenate/tcp_serializacion.h>
#include "../globals.h"

extern op_code global_memory_operation;
extern t_pcb* global_pcb_to_memory;

void *memoria_routine(void *config);
void page_fault_process(t_pcb* pcb, uint32_t seg_num, uint32_t page_num);

#endif /* MEMORIA_ROUTINE_H_ */