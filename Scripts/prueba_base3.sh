#! /usr/bin/bash
CONFIG=../Consola/Configs/consolaBase.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

# Consola 3
INSTRUCTIONS=../../geck-pruebas/BASE_3
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

../Consola/Debug/consola $ABS_C $ABS_I
