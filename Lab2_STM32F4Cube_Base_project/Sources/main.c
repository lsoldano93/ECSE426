/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : A program which showcases ADC and TIM3 under the new firmware
	                       The ADC
	* Authors						 : Ashraf Suyyagh, Luke Soldano, Xavier Agostini
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_gpio.h"
#include "supporting_functions.h"
#include "global_vars.h"

#define AVG_SLOPE (125 + 40) / ((3.6 - 1.8) * 1000)
#define ALARM_THRESHOLD_TEMP 60

typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;
float	V25 = AVG_SLOPE * 25;
int ticks = 0;

/* Private function prototypes -----------------------------------------------*/
void ADC_Config(void);
void SystemClock_Config(void);
void GPIO_Config(void);
float getTemp(void);
void displayTemp(float temp);
void lightDigit(int digit);
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);

/* Initialize -------------------------------------------------------------------*/
void ADC_Config(void) {

	ADC_InitTypeDef ADC1_itd;
	ADC_ChannelConfTypeDef ADC1_ch16;
	
	// Enable ADC clock
	__HAL_RCC_ADC1_CLK_ENABLE();
	
	// Initialize values for ADC1 init type def
	ADC1_itd.Resolution = ADC_RESOLUTION_12B;    									// 12 bit resolution      
	ADC1_itd.DataAlign = ADC_DATAALIGN_RIGHT;         						// Data alligns to right
	ADC1_itd.ScanConvMode = DISABLE;           
	ADC1_itd.EOCSelection = ADC_EOC_SINGLE_CONV;           				// Single conversion mode
	ADC1_itd.ContinuousConvMode = DISABLE;     
	ADC1_itd.DMAContinuousRequests = ADC_DMAACCESSMODE_DISABLED;  
	ADC1_itd.NbrOfConversion = 1;       													// Single conversion per call
	ADC1_itd.DiscontinuousConvMode = DISABLE;  
	ADC1_itd.NbrOfDiscConversion = 0;    
	
	// ***Check these two last values, not sure if they are right***
	ADC1_itd.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T8_CC1;       
	ADC1_itd.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  
	
	// Initialize values for ADC1 handle type def
	ADC1_Handle.Instance = ADC1;
	ADC1_Handle.Init = ADC1_itd;
	
	// Initialize values for temperature sensor (Temperature analog channel is Ch16 of ADC1)
	ADC1_ch16.Channel = ADC_CHANNEL_16;
	ADC1_ch16.Rank = 1;

	ADC1_ch16.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	ADC1_ch16.Offset = 0;
	
	// Configure temperature sensor peripheral 
	HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC1_ch16);
	

	
	
}
void GPIO_Config() {
	
	// WARNING: DO NOT USE PORT A13, PORT A14, or PORT B3
	/* GPIO Pin Mapping
		 Segment Ctrls {S1:PC1, S2:PC2, S3:PC4, S4:PC5}
		 DP Ctrl 	     {DP:PC6}
		 Segments      {A:PB0, B:PB1, C:PB2, D:PB11, E:PB12, F:PB13, G:PB14 */
	

	GPIO_InitTypeDef GPIO_InitB, GPIO_InitC;
	
	// Enable clocks for ports B & C
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	// TODO: Initialization still needs work
	GPIO_InitB.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitB.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitB.Pull = GPIO_PULLUP;
	GPIO_InitB.Speed =  GPIO_SPEED_FREQ_HIGH;
	//GPIO_Init.Alternate = ;
	
	
	GPIO_InitC.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitC.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitC.Pull = GPIO_PULLUP;
	GPIO_InitC.Speed =  GPIO_SPEED_FREQ_HIGH;
	//GPIO_Init.Alternate = ;
	
	HAL_GPIO_Init(GPIOB, &GPIO_InitB);
	HAL_GPIO_Init(GPIOC, &GPIO_InitC);
	
	
}
float getTemp() {
	//HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout)

	float VSENSE, temp;	
	HAL_ADC_Start(&ADC1_Handle);	

	VSENSE = HAL_ADC_PollForConversion(&ADC1_Handle, 200);
	HAL_ADC_GetValue(&ADC1_Handle); //gets the temperature voltage value
	HAL_ADC_Stop(&ADC1_Handle);
	//uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc); //error handling
	
	// Temperature (in °C) = {(VSENSE – V25) / AVG_SLOPE} + 25
	
	return temp = ((VSENSE - V25)/AVG_SLOPE) + 25;
}

void displayTemp(float temp){
	     
	/* GPIO Pin Mapping
	   Segment Ctrls {S1:PC1, S2:PC2, S3:PC4, S4:PC5}
		 DP Ctrl 	     {DP:PC6}
		 Segments      {A:PB0, B:PB1, C:PB2, D:PB11, E:PB12, F:PB13, G:PB14 */
	
	int displayValues, digit, tempValue4, tempValue3, tempValue2, tempValue1, i;
	
	for(i=4; i>0; i--) {
		switch(digit) {
			case 4:
				//state
				HAL_GPIO_WritePin(GPIOC, 1, GPIO_PIN_SET); //segment 4
				break;
			case 3:
				//state
				HAL_GPIO_WritePin(GPIOC, 2, GPIO_PIN_SET); //segment 3
				break;
			case 2:
				//state
				HAL_GPIO_WritePin(GPIOC, 4, GPIO_PIN_SET); //segment 2
				break;
			case 1:
				//state
				HAL_GPIO_WritePin(GPIOC,5, GPIO_PIN_SET); //segment 1
				break;
	
			default:
				break;
		}
		lightDigit(4);
		
		
	}
	// Determine how many values to display and what each individual value is
//	if (temp >= 100.0){
//		displayValues = 4;
//		tempValue4 = temp/100;
//		tempValue3 = temp/10 - tempValue4*10;
//		tempValue2 = temp - tempValue4*100 - tempValue3*10;
//		tempValue1 = (temp - tempValue4*100 - tempValue3*10 - tempValue2)*10;
//	}
//	else {
//		if (temp >= 10.0){
//				displayValues = 3;
//				tempValue3 = temp/10;
//				tempValue2 = temp - tempValue3*10;
//				tempValue1 = (temp - tempValue3*10 - tempValue2)*10;
//		}
//		else {
//				if (temp >= 1.0){
//						displayValues = 2;
//						tempValue2 = temp;
//						tempValue1 = (temp - tempValue2)*10;
//				}
//				else{
//					displayValues = 1;
//					tempValue1 = temp*10;
//				}
//		}
//	}
		
	// Set GPIO pins to dispaly segments
//	for (i=4; i > 0; i--){
//		
//		// Display nothing
//		if (i <= displayValues){
//			
//		}
//		//Segments      {A:PB0, B:PB1, C:PB2, D:PB11, E:PB12, F:PB13, G:PB14
//		//void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
//		//void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
//		
//		
//	}
	
	//segment controls
	// Segment Ctrls {S1:PC1, S2:PC2, S3:PC4, S4:PC5}
	
}
	

void lightDigit(int digit) {
	switch(digit) {
			case 0:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //g
				break;
			
			case 1:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //g

			case 2:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_RESET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g

			case 3:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g

			case 4:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g			

			case 5:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g
				break;
			
			case 6:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g

			case 7:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //g

			case 8:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g

			case 9:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_SET); //g			
			
			case 10:
				//state
				HAL_GPIO_WritePin(GPIOA, 0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOA, 1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOA, 2, GPIO_PIN_RESET); //c
				HAL_GPIO_WritePin(GPIOA, 11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOA, 12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOA, 13, GPIO_PIN_RESET); //g		
				
			default:
				break;
		}
}

int main(void)
{
	float temp;
	
	// Give ticks an initial value
	ticks = 0;
	
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
	ADC_Config();
	
	// Initialize ADC1 and check to make sure it happened
	HAL_ADC_Init(&ADC1_Handle);
	if (HAL_ADC_Init(&ADC1_Handle) != HAL_OK) Error_Handler(ADC_INIT_FAIL);


	while(1) {
		
		// Wait for ticks flag to be set high by SysTick
		if (ticks){
			
			// Ping ADC for reading and obtain temperature
			temp = getTemp();
			
			displayTemp(temp);
			printf("Tempertaure: %f \n",temp);
			
		
			// Set global tick variable to 0 and wait for SysTick to reset flag
			ticks = 0;
		}
	}
	
return NULL;
}



/** System Clock Configuration */
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
	
	/*Configures SysTick to provide 1ms interval interrupts.*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/* This function sets the source clock for the internal SysTick Timer to be the maximum,
	   in our case, HCLK is now 168MHz*/
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/**
   * @brief A function used to setup the ADC to sample Channel 16
   * @retval None
   */

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
