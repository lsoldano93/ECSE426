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


/* @brief Structure for the Kalman filter  */ 
typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

/* Private function prototypes -----------------------------------------------*/

/**  Assembly Kalmann filter function
   * @brief  Filters values to remove noisy fluctuations
	 * @param  Input value of measurements, array to output to, length of arrays, and kalman parameter struct
   * @retval Returns updated output array **/
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);

#endif
