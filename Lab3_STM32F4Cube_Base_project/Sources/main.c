/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program subroutine
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "supporting_functions.h"
#include "lis3dsh.h"
#include "global_vars.h"
#include "accelerometer.h"
#include "display_segment.h"
#include "keypad.h"

//#include "keypad.h"


/* Private variables ---------------------------------------------------------*/
int ticks = 0;
float upperBound, lowerBound;
float targetAngle = 50.0;

//array of size 3 that contains the accelerometer output
float accelerometer_out[3];

//interupt for accelerometer
//uint8_t ready_to_update_accelerometer = 0;

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

void init_TIM3(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim);
void SystemClock_Config	(void); 


/* Private functions ---------------------------------------------------------*/

void init_TIM3(void) {
	TIM_Base_InitTypeDef init_TIM;
	// enable clock for TIM3 
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	// TODO: Calibrate clock for proper frequency
	// Desired Rate = ClockFrequency / (prescaler * period)
	// Rate = 1000Hz, frequency = 42MHz 
	// need to setup period and prescaler
	//set rate to 500Hz
	
	//period is in MHz
	init_TIM.Period = 42;
	init_TIM.Prescaler = 2000;
	init_TIM.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	init_TIM.CounterMode = TIM_COUNTERMODE_UP;
	
	handle_tim3.Instance = TIM3;
	handle_tim3.Init = init_TIM;
	handle_tim3.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
	handle_tim3.Lock = HAL_UNLOCKED;
	handle_tim3.State = HAL_TIM_STATE_READY;

	HAL_TIM_Base_MspInit(&handle_tim3);
	
	
	
	HAL_TIM_Base_Init(&handle_tim3);
	HAL_TIM_Base_Start_IT(&handle_tim3);
		
	/* Configure NVIC */
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, 0,0);
	//HAL_NVIC_ClearPendingIRQ(TIM3_IRQn);
}

void display(float angle) {	
	
	upperBound = targetAngle + 5.0;
	lowerBound = targetAngle - 5.0;

	// Angle board down
	if(angle < lowerBound) draw_u();
	// Angle board up
	else if(angle > upperBound) draw_d();
	// Within derired range of +/-5 degrees; display user angle on display
	else draw_angle(angle);
 
}


int main(void){	
  
	float tmp;
	float angle;
	int count;
	int test;
	kalman_t kalmanPitch, kalmanRot;

	//Configure kalman filter for pitch
	kalmanPitch.q = 0.2;
	kalmanPitch.r = 2.1;
	kalmanPitch.x = 0.0;
	kalmanPitch.p = 0.0;
	kalmanPitch.k = 0.0;
	
	//Configure kalman filter for roll

	kalmanRot.q = 0.2;
	kalmanRot.r = 2.1;
	kalmanRot.x = 0.0;
	kalmanRot.p = 0.0;
	kalmanRot.k = 0.0;
	
	/* MCU Configuration----------------------------------------------------------*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();
	
  /* Initialize all configured peripherals and timers */
  init_TIM3();
	init_accelerometer(); //initialize accelerometer
	init_display();
	//init_keypad();
	
//	__HAL_GPIO_EXTI_GENERATE_SWIT(EXTI0_IRQn); //create software interrupt
	
	ready_to_update_accelerometer = 0; //set accelerometer initally to 0
	tim3_flag = 0; //set timer flag intially to 0
	
	
	printf("Start\n");

	//run when interrupt received
	while (1){
		test = get_key();
		printf("key entered: %d\n", test);
		//
		if(ready_to_update_accelerometer == 1) {
			
			LIS3DSH_ReadACC(accelerometer_out);
			
			//reset interrupt
			//
			ready_to_update_accelerometer = 0;
			
			//update real accelerometer values
			update_accel_values(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]);
			
			//filtering
			//Kalmanfilter_asm(pitch, output, ARRAY_LENGTH, &kalmanPitch);
			//Kalmanfilter_asm(roll, output, ARRAY_LENGTH, &kalmanRot);
			
			//use pitch
			angle = fabs(calc_roll_angle());
			printf("Pitch: %f, Rotation: %f\n", calc_pitch_angle(), calc_roll_angle());
		}
		
		
		//run when tim3 interupt goes off
		if(tim3_flag == 1) {	
			
			tim3_flag = 0;
			
			
			if(count >= 10) {				
				tmp = angle;
				count = 0;
			}
			count++;
			
		}
		
		display(tmp);
		
	}
	
	return 0;
}

/**
  * @brief  Input Capture callback in non blocking mode 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	
	/* Prevent unused argument(s) compilation warning */
  __IO uint32_t tmpreg = 0x00;
  UNUSED(tmpreg); 
	
	if(GPIO_Pin == GPIO_PIN_0) {
		LIS3DSH_ReadACC(accelerometer_out);
	}	
}	
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
	/* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
	
}

/** System Clock Configuration*/
void SystemClock_Config(void){

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType 	= RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState 			 	= RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState 		= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 	= RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM 				= 8;
  RCC_OscInitStruct.PLL.PLLN 				= 336;
  RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ 				= 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};

  RCC_ClkInitStruct.ClockType 			= RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource 		= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 	= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 	= RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider 	= RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5)!= HAL_OK){Error_Handler(RCC_CONFIG_FAIL);};
	
	/*Configures SysTick to provide 1ms interval interrupts. SysTick is already 
	  configured inside HAL_Init, I don't kow why the CubeMX generates this call again*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line){
}
#endif

