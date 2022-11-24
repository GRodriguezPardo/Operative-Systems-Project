#! /usr/bin/bash

# Consola 1
CONFIG=../Consola/Configs/consola1.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_1
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 2
CONFIG=../Consola/Configs/consola2.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_2
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 3
CONFIG=../Consola/Configs/consola3.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_3
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 4
CONFIG=../Consola/Configs/consola4.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_4
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 5
CONFIG=../Consola/Configs/consola5.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_5
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"

# Consola 6
CONFIG=../Consola/Configs/consola6.config
ABS_C=$(dirname $(readlink -e $CONFIG))/$(basename $CONFIG)

INSTRUCTIONS=../../geck-pruebas/ESTABILIDAD_6
ABS_I=$(dirname $(readlink -e $INSTRUCTIONS))/$(basename $INSTRUCTIONS)

terminator -e "../Consola/Debug/consola $ABS_C $ABS_I"
