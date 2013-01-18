################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include src/subdir.mk
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
memory_test.elf.size \

ELFCHECK += \
memory_test.elf.elfcheck \


# All Target
all: memory_test.elf secondary-outputs

# Tool invocations
memory_test.elf: $(OBJS) ../src/lscript.ld $(USER_OBJS)
	@echo Building target: $@
	@echo Invoking: MicroBlaze gcc linker
	mb-gcc -Wl,--no-relax -Wl,-T -Wl,../src/lscript.ld -L../../standalone_bsp/microblaze_0/lib -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.40.a -mno-xl-soft-mul -mxl-multiply-high -mhard-float -mxl-float-convert -mxl-float-sqrt -o"memory_test.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo Finished building target: $@
	@echo ' '

memory_test.elf.size: memory_test.elf
	@echo Invoking: MicroBlaze Print Size
	mb-size memory_test.elf  |tee "memory_test.elf.size"
	@echo Finished building: $@
	@echo ' '

memory_test.elf.elfcheck: memory_test.elf
	@echo Invoking: Xilinx ELF Check
	elfcheck memory_test.elf -hw ../../hw_hw_platform/system.xml -pe microblaze_0  |tee "memory_test.elf.elfcheck"
	@echo Finished building: $@
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES)$(ELFSIZE)$(ELFCHECK)$(S_UPPER_DEPS) memory_test.elf
	-@echo ' '

secondary-outputs: $(ELFSIZE) $(ELFCHECK)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
