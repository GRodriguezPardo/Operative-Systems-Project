################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

H_SRCS=$(shell find ./ -iname "*.h" | tr '\n' ' ' )

# All of the sources participating in the build are defined here
-include sources.mk
-include objects/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

ifneq ($(shell id -un),root)
SUDO=sudo
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := libthesenate
BUILD_ARTIFACT_EXTENSION := so
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: libthesenate

# Tool invocations
libthesenate: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -shared -o "$(BUILD_ARTIFACT)" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	@echo 'Moving library to standard place'
	$(SUDO) cp -u ./$(BUILD_ARTIFACT) /usr/lib
	$(SUDO) cp --parents -u $(H_SRCS) /usr/include
	@echo 'Finished: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BUILD_ARTIFACT)
	-$(SUDO) $(RM) /usr/lib/$(BUILD_ARTIFACT)
	-$(SUDO) $(RM) /usr/lib/thesenate
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
