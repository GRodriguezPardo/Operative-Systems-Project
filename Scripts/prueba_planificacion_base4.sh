#! /usr/bin/bash
CONFIG=../Consola/Configs/consolaPlanificacion.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

# Consola 4
INSTRUCTIONS=../../geck-pruebas/PLANI_BASE_4
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

../Consola/Debug/consola $ABS_C $ABS_I
