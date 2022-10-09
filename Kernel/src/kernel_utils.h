#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <stdbool.h>
#include "globals.h"

void init_globals_kernel(int grado_multiprogramacion);
void finalizar_kernel(t_config* config, t_log* logger);

bool pcb_add_comparator(void *arg1, void *arg2);
void pcb_element_destroyer(void *arg);

void logger_monitor_info(t_log* logger, const char* message);
void logger_monitor_error(t_log* logger, const char* message);

bool configurar_algoritmo(char* algortimo);

extern uint32_t search_for_id_buffer;
bool search_for_id(void *param);

#endif /* KERNEL_UTILS_H_ */