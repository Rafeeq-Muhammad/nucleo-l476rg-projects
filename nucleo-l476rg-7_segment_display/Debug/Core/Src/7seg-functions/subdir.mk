################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/7seg-functions/7seg-functions.c 

OBJS += \
./Core/Src/7seg-functions/7seg-functions.o 

C_DEPS += \
./Core/Src/7seg-functions/7seg-functions.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/7seg-functions/%.o: ../Core/Src/7seg-functions/%.c Core/Src/7seg-functions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L476xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-7seg-2d-functions

clean-Core-2f-Src-2f-7seg-2d-functions:
	-$(RM) ./Core/Src/7seg-functions/7seg-functions.d ./Core/Src/7seg-functions/7seg-functions.o

.PHONY: clean-Core-2f-Src-2f-7seg-2d-functions

