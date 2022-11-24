#! /usr/bin/bash
CONFIG=../Consola/Configs/consola.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

# Consola 1
INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_1
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 2
INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_2
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 3
INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_3
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 4
INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_4
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 5
INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_5
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 6
INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_6
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"
