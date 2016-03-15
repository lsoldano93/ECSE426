/*******************************************************************************
  * @file    main.c
  * @author  Ashraf Suyyagh
	* @version V1.2.0
  * @date    17-January-2016
  * @brief   This file demonstrates flasing one LED at an interval of one second
	*          RTX based using CMSIS-RTOS 
  ******************************************************************************
  */

#include "main.h"


/**
	These lines are mandatory to make CMSIS-RTOS RTX work with te new Cube HAL
*/
#ifdef RTE_CMSIS_RTOS_RTX
extern uint32_t os_time;

uint32_t HAL_GetTick(void) {
  return os_time; 
}
#endif

/**
  * System Clock Configuration
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the
     device is clocked below the maximum system frequency (see datasheet). */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}


/**  Causes the display of a temperature or angle value
   * @brief  Will show the proper float on the display and will flash if temperature alarm sounded
	 * @param  Float of temperature/angle to be displayed
	 * @param	 Boolean that is false for temperature display and true for angle display
   * @param	 Boolean that indicates whether or not temperature alarm is sounding **/
void display(float displayValue, uint8_t displayBool, uint8_t alarmBool){
	
	uint8_t i;
	
	// Reset all pins and set delay to show noticeable blink if alarm active
	if (alarmBool){
		for(i=0; i<4; i++){
			selectDigit(i);
			reset();
		}
		
		osDelay(100);
	}
	
	if (displayBool) draw_angle(displayValue);
	else draw_temperature(displayValue);
	
	return;
}


/**  Timer3 initialization function
   * @brief  Function to initialize timer 3 **/
void init_TIM3(void) {
	
	TIM_Base_InitTypeDef init_TIM;
	
	// Enable clock for TIM3 
	__HAL_RCC_TIM3_CLK_ENABLE();
	
	// Desired Rate = ClockFrequency / (prescaler * period)
	// Rate = 1000Hz, frequency = 42MHz																		 TODO: FIX to 500 HZ
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

/**
  * @brief  This function handles accelerometer interrupt requests
  * @param  None
  * @retval None
  */
//void EXTI0_IRQHandler(void){
//	
//	// Listen to pin 0
//	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
//	
//	// Flag for interrupt
//	ready_to_update_accelerometer = 1;

//}

/**
  * @}
  */ 

void TIM3_IRQHandler(void) {
	
	HAL_TIM_IRQHandler(&handle_tim3);

	tim3_flag = 1;

	if(TimmingDelay !=0) {
			TimmingDelay --;
	}
	
}





/**  Runs user interface system
   * @brief  Initializes threads and peripherals to maintian a RTOS for the user **/
int main (void) {
	
	
	
  osKernelInitialize();                     /* initialize CMSIS-RTOS          */
  HAL_Init();                               /* Initialize the HAL Library     */
  SystemClock_Config();                     /* Configure the System Clock     */
	
	printf("Beginning Program\n");
	
	/* User codes goes here*/
	init_TIM3();
	initialize_LED_IO();                       /* Initialize LED GPIO Buttons    */

	start_Thread_LED();					 /* Create LED thread              */	
  
	//initialize and configure temperature ADC
	ADC_config();
	
	start_Thread_TempSensor();
	
	// TODO: Add accelerometer thread initialization and start
	
	init_accelerometer();
	start_Thread_Accelerometer();
	
	// TODO: Add key press thread initialization and start
	
	// TODO: Add display thread initialization and start
	//init_display();

	/* User codes ends here*/
  
	osKernelStart();                          /* start thread execution         */
	

	

	
}
