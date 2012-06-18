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
raw_apps.elf.size \

ELFCHECK += \
raw_apps.elf.elfcheck \


# All Target
all: raw_apps.elf secondary-outputs

# Tool invocations
raw_apps.elf: $(OBJS) ../raw_apps.ld $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze gcc linker
	mb-gcc -Wl,-T -Wl,../raw_apps.ld -L../../raw_bsp/microblaze_0/lib -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.10.a -mno-xl-soft-mul -mxl-multiply-high -mhard-float -mxl-float-convert -mxl-float-sqrt -o"raw_apps.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

raw_apps.elf.size: raw_apps.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size raw_apps.elf  |tee "raw_apps.elf.size"
	@echo Finished building: $@
	@echo ' '

raw_apps.elf.elfcheck: raw_apps.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck raw_apps.elf -hw ../../hw_hw_platform/system.xml -pe microblaze_0  |tee "raw_apps.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) raw_apps.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
