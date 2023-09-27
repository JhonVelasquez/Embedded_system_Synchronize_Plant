################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/BSP/Components/lan8742/lan8742.c 

OBJS += \
./Drivers/BSP/Components/lan8742.o 

C_DEPS += \
./Drivers/BSP/Components/lan8742.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/lan8742.o: C:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/BSP/Components/lan8742/lan8742.c Drivers/BSP/Components/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F746xx -c -I../Core/Inc -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/STM32F7xx_HAL_Driver/Inc -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/CMSIS/Device/ST/STM32F7xx/Include -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/CMSIS/Include -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/FreeRTOS/Source/include -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../LWIP/App -I../LWIP/Target -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/system -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Drivers/BSP/Components/lan8742 -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/netif/ppp -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/lwip -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/lwip/apps -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/lwip/priv -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/lwip/prot -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/netif -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/compat/posix -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/compat/posix/arpa -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/compat/posix/net -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/compat/posix/sys -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/src/include/compat/stdc -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/Third_Party/LwIP/system/arch -I../USB_DEVICE/App -I../USB_DEVICE/Target -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/ST/STM32_USB_Device_Library/Core/Inc -IC:/Users/jvlsq/STM32Cube/Repository/STM32Cube_FW_F7_V1.17.0/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components

clean-Drivers-2f-BSP-2f-Components:
	-$(RM) ./Drivers/BSP/Components/lan8742.cyclo ./Drivers/BSP/Components/lan8742.d ./Drivers/BSP/Components/lan8742.o ./Drivers/BSP/Components/lan8742.su

.PHONY: clean-Drivers-2f-BSP-2f-Components

