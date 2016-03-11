#ifndef MAIN_H
#define MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "supporting_functions.h"
#include "lis3dsh.h"
#include "global_vars.h"
#include "accelerometer.h"
#include "display_segment.h"
#include "keypad.h"

/* Private typedef -----------------------------------------------------------*/

/* @brief Structure for the Kalman filter  */ 
typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**  Assembly Kalmann filter function
   * @brief  Filters values to remove noisy fluctuations
	 * @param  Input value of measurements, array to output to, length of arrays, and kalman parameter struct
   * @retval Returns updated output array **/
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);

/**  Timer3 initialization function
   * @brief  Function to initialize timer 3 **/
void init_TIM3(void);

/**  Function for displaying the angle/motion instructions as indicated by lab three documentation
   * @brief  Displays angle if board within 5 degrees of  target, o/w displays directional instructions
	 * @param  Target angle of orientation **/
void display(float angle);

/**  Main function
   * @brief  Initializes clock and peripherals, then runs high level program to allow user interface
   * @retval Returns 0 upon succesful completion **/
int main(void);

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

/**
  * @brief  Callback for timer 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim);

/**  System clock configuration
   * @brief  System Clock Configuration **/
void SystemClock_Config	(void); 

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line);

#endif