#! /usr/bin/bash
CONFIG=../Consola/Configs/consola.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

# Consola 1
INSTRUCTIONS=../../geck-pruebas/BASE_1
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 2
INSTRUCTIONS=../../geck-pruebas/BASE_2
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 3
INSTRUCTIONS=../../geck-pruebas/BASE_3
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"
