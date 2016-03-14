#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#include "cmsis_os.h"

/* Public variables ----------------------------------------------------------*/

// Flags
extern uint8_t key_state;
extern uint8_t tilt_state; 

// Mutexes
extern osMutexId temperatureMutex;

// Shared variables
extern float temperatureValue;

//tim3
extern uint8_t tim3_flag;
extern uint16_t tim3_ticks;
extern uint32_t TimmingDelay;

// Timer handler
extern TIM_HandleTypeDef handle_tim3;

#endif
