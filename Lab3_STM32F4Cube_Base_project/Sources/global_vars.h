#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

#define TIM3_BOUND 999

/* Public variables ----------------------------------------------------------*/

// Flags
extern uint8_t ready_to_update_accelerometer;
extern uint8_t tim3_flag;
extern uint16_t tim3_ticks;
extern uint32_t TimmingDelay;

// Timer handler
extern TIM_HandleTypeDef handle_tim3;


#endif