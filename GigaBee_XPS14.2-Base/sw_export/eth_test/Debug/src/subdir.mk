################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/1wire.c \
../src/eth_arp.c \
../src/eth_base.c \
../src/eth_hw.c \
../src/eth_icmp.c \
../src/eth_udp.c \
../src/eth_udp_tftp.c \
../src/main.c \
../src/spi_flash.c 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/1wire.o \
./src/eth_arp.o \
./src/eth_base.o \
./src/eth_hw.o \
./src/eth_icmp.o \
./src/eth_udp.o \
./src/eth_udp_tftp.o \
./src/main.o \
./src/spi_flash.o 

C_DEPS += \
./src/1wire.d \
./src/eth_arp.d \
./src/eth_base.d \
./src/eth_hw.d \
./src/eth_icmp.d \
./src/eth_udp.d \
./src/eth_udp_tftp.d \
./src/main.d \
./src/spi_flash.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: MicroBlaze gcc compiler
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -Wl,--no-relax -I../../standalone_bsp_0/microblaze_0/include -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.40.a -mno-xl-soft-mul -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


