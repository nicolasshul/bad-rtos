#ifndef MCU_DEVICE_H
#define MCU_DEVICE_H

#if defined(STM32L432xx) || defined(STM32L4)
    #include "stm32l4xx.h"
#else
    #error "Target MCU not defined"
#endif

#endif