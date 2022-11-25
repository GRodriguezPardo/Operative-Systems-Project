#! /usr/bin/bash

# Consola 2
CONFIG=../Consola/Configs/consolaEstabilidad1.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_2
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

../Consola/Debug/consola $ABS_C $ABS_I