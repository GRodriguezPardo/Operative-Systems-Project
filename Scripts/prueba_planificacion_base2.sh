#! /usr/bin/bash
CONFIG=../Consola/Configs/consolaPlanificacion.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

# Consola 2
INSTRUCTIONS=../../geck-pruebas/PLANI_BASE_2
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

../Consola/Debug/consola $ABS_C $ABS_I