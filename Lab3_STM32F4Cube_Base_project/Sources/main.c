/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program subroutine
	* Author						 : Ashraf Suyyagh, Xavier Agostini, Luke Soldano
	* Version            : 1.0.1
	* Date							 : March 11th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/

float targetAngle = 90.0;
float accelerometer_out[3]; 		// Array being updated by accelerometer.c

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**  Timer3 initialization function
   * @brief  Function to initialize timer 3 **/
void init_TIM3(void) {
	
	TIM_Base_InitTypeDef init_TIM;
	
	// Enable clock for TIM3 
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	// TODO: Calibrate clock for proper frequency
	// Desired Rate = ClockFrequency / (prescaler * period)
	// Rate = 1000Hz, frequency = 42MHz 
	// need to setup period and prescaler
	// set rate to 500Hz
	
	// Initialize timer 3 initialization struct 
	init_TIM.Period = 42;			 								// Period is in MHz
	init_TIM.Prescaler = 2000;
	init_TIM.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	init_TIM.CounterMode = TIM_COUNTERMODE_UP;
	
	// Initialize timer 3 handle struct
	handle_tim3.Instance = TIM3;
	handle_tim3.Init = init_TIM;
	handle_tim3.Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
	handle_tim3.Lock = HAL_UNLOCKED;
	handle_tim3.State = HAL_TIM_STATE_READY;

	// Initialize timer 3 handle and enable interrupts
	HAL_TIM_Base_MspInit(&handle_tim3);
	HAL_TIM_Base_Init(&handle_tim3);
	HAL_TIM_Base_Start_IT(&handle_tim3);
		
	// Configure NVIC 
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	HAL_NVIC_SetPriority(TIM3_IRQn, 0,0);
	//HAL_NVIC_ClearPendingIRQ(TIM3_IRQn);
	
}


/**  Function for displaying the angle/motion instructions as indicated by lab three documentation
   * @brief  Displays angle if board within 5 degrees of  target, o/w displays directional instructions
	 * @param  Target angle of orientation **/
void display(float angle) {	
	
	float upperBound, lowerBound;
	
	upperBound = targetAngle + 5.0;
	lowerBound = targetAngle - 5.0;

	// Angle board down
	if(angle < lowerBound) draw_u();
	// Angle board up
	else if(angle > upperBound) draw_d();
	// Within derired range of +/-5 degrees; display user angle on display
	else draw_angle(targetAngle);
 
}


/**  Main function
   * @brief  Initializes clock and peripherals, then runs high level program to allow user interface
   * @retval Returns 0 upon succesful completion **/
int main(void){	
  
	float angle;
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
	
	// MCU Configuration
  HAL_Init();

  // Configure the system clock 
  SystemClock_Config();
	
  // Initialize all configured peripherals and timers
  init_TIM3();
	init_accelerometer(); 
	init_display();
	
	//	__HAL_GPIO_EXTI_GENERATE_SWIT(EXTI0_IRQn); //create software interrupt
	
	// Set global flags to initial value of 0
	ready_to_update_accelerometer = 0; 
	tim3_flag = 0;
	
	printf("Begin User Interface\n");

	while (1){
		
		// Read for key press
		if ((test = handle_key_press()) != -1 && test <= 180) {
			targetAngle = test;
			printf("Angle entered: %d\n", test);
		}
		else if (test != -1 && test > 180) printf("Angle (%d) invalid \n", test);
		
		// If accelerometer interrupt received, accquire new values and update variables
		if(ready_to_update_accelerometer == 1) {
			
			// Lower interrupt flag
			ready_to_update_accelerometer = 0;
			
			// Update real accelerometer values
			update_accel_values(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]);
			
			// Kalman filtering
			//Kalmanfilter_asm(pitch, output, ARRAY_LENGTH, &kalmanPitch);
			//Kalmanfilter_asm(roll, output, ARRAY_LENGTH, &kalmanRot);
			
			angle = calc_roll_angle();
			
			//printf("%f\n", angle);
			//printf("%f,%f,%f\n", accelerometer_out[0],accelerometer_out[1],accelerometer_out[2]);
			
		} 
		
		display(angle);
		
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
	
	// If callback regards GPIO pin associated with external accel interrupt, read accel
	if(GPIO_Pin == GPIO_PIN_0) {
		LIS3DSH_ReadACC(accelerometer_out);
	}	
}	


/**
  * @brief  Callback for timer 
  * @param  htim: pointer to a TIM_HandleTypeDef structure that contains
  *                the configuration information for TIM module.
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
	/* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
	
}


/**  System clock configuration
   * @brief  System Clock Configuration **/
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

