################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ELFSIZE += \
socket_apps.elf.size \

ELFCHECK += \
socket_apps.elf.elfcheck \


# All Target
all: socket_apps.elf secondary-outputs

# Tool invocations
socket_apps.elf: $(OBJS) ../socket_apps.ld $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze gcc linker
	mb-gcc -Wl,-T -Wl,../socket_apps.ld -L../../socket_bsp/microblaze_0/lib -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.20.a -mno-xl-soft-mul -mxl-multiply-high -mhard-float -mxl-float-convert -mxl-float-sqrt -o"socket_apps.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

socket_apps.elf.size: socket_apps.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size socket_apps.elf  |tee "socket_apps.elf.size"
	@echo Finished building: $@
	@echo ' '

socket_apps.elf.elfcheck: socket_apps.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck socket_apps.elf -hw ../../hw_hw_platform/system.xml -pe microblaze_0  |tee "socket_apps.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) socket_apps.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
