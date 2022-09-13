################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
./thesenate/tcp_client.c \
./thesenate/tcp_server.c \
./thesenate/tcp_serializacion.c

C_DEPS += \
./objects/tcp_client.d \
./objects/tcp_server.d \
./objects/tcp_serializacion.d

OBJS += \
./objects/tcp_client.o \
./objects/tcp_server.o \
./objects/tcp_serializacion.o


# Each subdirectory must supply rules for building sources it contributes
./objects/%.o: ./thesenate/%.c objects/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -fpic -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

clean: clean-src

clean-src:
	-$(RM) ./objects/tcp_client.d ./objects/tcp_server.d ./objects/tcp_serializacion.d ./objects/tcp_client.o ./objects/tcp_server.o ./objects/tcp_serializacion.o

.PHONY: clean-src

