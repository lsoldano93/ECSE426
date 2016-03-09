#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

/* Public variables ----------------------------------------------------------*/
#define TIM3_BOUND = 999;
extern uint8_t ready_to_update_accelerometer;
extern TIM_HandleTypeDef handle_tim3;
extern float angle;
extern float targetAngle;// = 50.0



#endif