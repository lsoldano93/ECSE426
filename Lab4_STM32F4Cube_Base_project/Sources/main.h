#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Components.h"             // Component selection
#include "accelerometer.h"
#include "keypad.h"
#include "display_segment.h"
#include "Thread_TempSensor.h"
#include "global_vars.h"

extern void initialize_LED_IO (void);
extern void start_Thread_LED (void);
extern void Thread_LED(void const *argument);
extern osThreadId tid_Thread_LED;
extern osThreadId tid_Thread_TempSensor; 
void TIM3_IRQHandler(void);
void Delay(uint32_t time);

osMutexId temperatureMutex;
uint8_t tim3_flag;
uint16_t tim3_ticks;
TIM_HandleTypeDef handle_tim3;
uint32_t TimmingDelay;
float accelerometer_out[3];
#endif