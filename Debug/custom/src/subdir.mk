################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/src/MODBUS.c \
../custom/src/declaration.c \
../custom/src/file_system.c \
../custom/src/function.c \
../custom/src/main.c \
../custom/src/memory.c \
../custom/src/mqtt.c \
../custom/src/sms.c \
../custom/src/watchdog_task.c 

OBJS += \
./custom/src/MODBUS.o \
./custom/src/declaration.o \
./custom/src/file_system.o \
./custom/src/function.o \
./custom/src/main.o \
./custom/src/memory.o \
./custom/src/mqtt.o \
./custom/src/sms.o \
./custom/src/watchdog_task.o 

C_DEPS += \
./custom/src/MODBUS.d \
./custom/src/declaration.d \
./custom/src/file_system.d \
./custom/src/function.d \
./custom/src/main.d \
./custom/src/memory.d \
./custom/src/mqtt.d \
./custom/src/sms.d \
./custom/src/watchdog_task.d 


# Each subdirectory must supply rules for building sources it contributes
custom/src/%.o: ../custom/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__CUSTOMER_CODE__ -I"${GCC_PATH}\arm-none-eabi\include" -I"E:\Deepak\PROJECTS\RS485_TO_GSM\REV_TOT_EFM\RS-485\include" -I"E:\Deepak\PROJECTS\RS485_TO_GSM\REV_TOT_EFM\RS-485\ril\inc" -I"E:\Deepak\PROJECTS\RS485_TO_GSM\REV_TOT_EFM\RS-485\custom\config" -I"E:\Deepak\PROJECTS\RS485_TO_GSM\REV_TOT_EFM\RS-485\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


