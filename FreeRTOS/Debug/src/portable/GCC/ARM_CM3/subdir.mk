################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/portable/GCC/ARM_CM3/port.c 

C_DEPS += \
./src/portable/GCC/ARM_CM3/port.d 

OBJS += \
./src/portable/GCC/ARM_CM3/port.o 


# Each subdirectory must supply rules for building sources it contributes
src/portable/GCC/ARM_CM3/%.o: ../src/portable/GCC/ARM_CM3/%.c src/portable/GCC/ARM_CM3/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_LPCOPEN -D__LPC15XX__ -D__NEWLIB__ -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/lpc_board_nxp_lpcxpresso_1549/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/lpc_chip_15xx/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/src/include" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/src/portable/GCC/ARM_CM3" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -D__NEWLIB__ -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-portable-2f-GCC-2f-ARM_CM3

clean-src-2f-portable-2f-GCC-2f-ARM_CM3:
	-$(RM) ./src/portable/GCC/ARM_CM3/port.d ./src/portable/GCC/ARM_CM3/port.o

.PHONY: clean-src-2f-portable-2f-GCC-2f-ARM_CM3

