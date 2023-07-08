################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/main.c 

OBJS += \
./custom/main.o 

C_DEPS += \
./custom/main.d 


# Each subdirectory must supply rules for building sources it contributes
custom/%.o: ../custom/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__CUSTOMER_CODE__ -I"${GCC_PATH}\arm-none-eabi\include" -I"F:\OPEN_CPU\Flowmeter_GSM_M66\M66_OpenCPU_GS3_SDK_V2.3_Eclipse\include" -I"F:\OPEN_CPU\Flowmeter_GSM_M66\M66_OpenCPU_GS3_SDK_V2.3_Eclipse\ril\inc" -I"F:\OPEN_CPU\Flowmeter_GSM_M66\M66_OpenCPU_GS3_SDK_V2.3_Eclipse\custom\config" -I"F:\OPEN_CPU\Flowmeter_GSM_M66\M66_OpenCPU_GS3_SDK_V2.3_Eclipse\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


