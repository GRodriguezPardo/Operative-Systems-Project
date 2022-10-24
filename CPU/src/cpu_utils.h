#ifndef KERNEL_UTILS_H_
#define KERNEL_UTILS_H_

#include <commons/config.h>
#include <commons/log.h>
#include <stdbool.h>
#include "globals.h"

void init_globals_cpu();
void finalizar_cpu(t_config* config, t_log* logger);
void logger_cpu_info(t_log* logger, const char* message);
void logger_cpu_error(t_log* logger, const char* message);

#endif