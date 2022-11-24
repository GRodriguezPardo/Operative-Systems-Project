#! /usr/bin/bash

# Consola 1
CONFIG=../Consola/Configs/consola1.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/MEMORIA_1
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 2
CONFIG=../Consola/Configs/consola2.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/MEMORIA_2
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 3
CONFIG=../Consola/Configs/consola3.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/MEMORIA_3
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"
