#! /usr/bin/bash

# Consola 1
CONFIG=../Consola/Configs/consolaTLB1.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/TLB_1
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

../Consola/Debug/consola $ABS_C $ABS_I
