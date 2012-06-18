################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../dispatch.c \
../echo.c \
../http_response.c \
../main.c \
../platform.c \
../platform_fs.c \
../platform_gpio.c \
../prot_malloc.c \
../rxperf.c \
../tftpserver.c \
../tftputils.c \
../txperf.c \
../urxperf.c \
../utxperf.c \
../web_utils.c \
../webserver.c 

LD_SRCS += \
../raw_apps.ld 

OBJS += \
./dispatch.o \
./echo.o \
./http_response.o \
./main.o \
./platform.o \
./platform_fs.o \
./platform_gpio.o \
./prot_malloc.o \
./rxperf.o \
./tftpserver.o \
./tftputils.o \
./txperf.o \
./urxperf.o \
./utxperf.o \
./web_utils.o \
./webserver.o 

C_DEPS += \
./dispatch.d \
./echo.d \
./http_response.d \
./main.d \
./platform.d \
./platform_fs.d \
./platform_gpio.d \
./prot_malloc.d \
./rxperf.d \
./tftpserver.d \
./tftputils.d \
./txperf.d \
./urxperf.d \
./utxperf.d \
./web_utils.d \
./webserver.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo Building file: $<
	@echo Invoking: MicroBlaze gcc compiler
	mb-gcc -Wall -O0 -g3 -I../../raw_bsp/microblaze_0/include -c -fmessage-length=0 -mlittle-endian -mxl-barrel-shift -mxl-pattern-compare -mno-xl-soft-div -mcpu=v8.10.a -mno-xl-soft-mul -mxl-multiply-high -mhard-float -mxl-float-convert -mxl-float-sqrt -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


