################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/cr_cpp_config.cpp \
../src/cr_startup_lpc15xx.cpp \
../src/lab1_01.cpp 

C_SRCS += \
../src/crp.c \
../src/heap_lock_monitor.c \
../src/sysinit.c 

CPP_DEPS += \
./src/cr_cpp_config.d \
./src/cr_startup_lpc15xx.d \
./src/lab1_01.d 

C_DEPS += \
./src/crp.d \
./src/heap_lock_monitor.d \
./src/sysinit.d 

OBJS += \
./src/cr_cpp_config.o \
./src/cr_startup_lpc15xx.o \
./src/crp.o \
./src/heap_lock_monitor.o \
./src/lab1_01.o \
./src/sysinit.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -std=c++11 -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M3 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC15XX__ -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/lpc_board_nxp_lpcxpresso_1549/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/lpc_chip_15xx/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/src/include" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/src/portable/GCC/ARM_CM3" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=c11 -DDEBUG -D__CODE_RED -D__NEWLIB__ -DCORE_M3 -D__USE_LPCOPEN -DCPP_USE_HEAP -D__LPC15XX__ -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/lpc_board_nxp_lpcxpresso_1549/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/lpc_chip_15xx/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/inc" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/src/include" -I"/Users/DBY/Metro_SemesterV-VI/ARM-FREERTOS/Worshop/FreeRTOS/src/portable/GCC/ARM_CM3" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/cr_cpp_config.d ./src/cr_cpp_config.o ./src/cr_startup_lpc15xx.d ./src/cr_startup_lpc15xx.o ./src/crp.d ./src/crp.o ./src/heap_lock_monitor.d ./src/heap_lock_monitor.o ./src/lab1_01.d ./src/lab1_01.o ./src/sysinit.d ./src/sysinit.o

.PHONY: clean-src

