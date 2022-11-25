#! /usr/bin/bash

# Consola 3
CONFIG=../Consola/Configs/consolaEstabilidad2.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_3
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

../Consola/Debug/consola $ABS_C $ABS_I