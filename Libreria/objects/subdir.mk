################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS = $(shell find ./thesenate/ -iname "*.c" | tr '\n' ' ')

C_SRCS_AUX = $(patsubst ./thesenate/%,./objects/%,$(C_SRCS))

C_DEPS = $(C_SRCS_AUX:.c=.d)

OBJS = $(C_SRCS_AUX:.c=.o)


# Each subdirectory must supply rules for building sources it contributes
./objects/%.o: ./thesenate/%.c objects/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -fpic -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

clean: clean-src

clean-src:
	-$(RM) $(C_DEPS) $(OBJS)

.PHONY: clean-src

