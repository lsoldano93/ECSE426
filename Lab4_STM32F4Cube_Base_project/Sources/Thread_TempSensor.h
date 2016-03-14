#ifndef THREAD_TEMPSENSOR_H
#define THREAD_TEMPSENSOR_H

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"                   
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "global_vars.h"


// TODO: Determine these values
#define tempKalman_q 0.1
#define tempKalman_r 2.0
#define tempKalman_x 25.0
#define tempKalman_p 0.0
#define tempKalman_k 0.0

/* Private typedef -----------------------------------------------------------*/

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

int start_Thread_TempSensor(void);
void Thread_TempSensor (void const *argument);  
void updateTemp(void);
void initialize_Temp_Thread(void);
int timer(__IO uint32_t time);
void Delay(uint32_t time);

#endif