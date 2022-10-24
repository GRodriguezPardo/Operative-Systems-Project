################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS_UTILS += \
../src/cpu_utils.c \
$(shell find ../src/functions/ -iname "*.c" | tr '\n' ' ')

C_SRCS_UTILS_AUX = $(patsubst ../%,./%,$(C_SRCS_UTILS))

C_SRCS += \
../src/cpu.c \
$(C_SRCS_UTILS)


C_DEPS += \
./src/cpu.d \
$(C_SRCS_UTILS_AUX:.c=.d)

OBJS += \
./src/cpu.o \
$(C_SRCS_UTILS_AUX:.c=.o)


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/functions/%.o: ../src/functions/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) $(C_DEPS) $(OBJS)

.PHONY: clean-src

