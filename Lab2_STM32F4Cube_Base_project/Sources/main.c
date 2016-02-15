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

typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;

/* Private function prototypes -----------------------------------------------*/
void config_ADC_temp(void);
void SystemClock_Config	(void);
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);

/* Initialize -------------------------------------------------------------------*/
void config_ADC_temp(void) {

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
	ADC1_itd.NbrOfConversion = 1;       													// Single conversion (per call?)
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
void gpioConfig() {
	//PC0-PC3: Segments controls 1,2,3,4
	//PA0: Dp
	//PA1-PA7: A-G
	

	GPIO_InitTypeDef GPIO_InitA, GPIO_InitC;
	
	// Enable clocks for ports A & B
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	//Initialization still needs work
	
	GPIO_InitA.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitA.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitA.Pull = GPIO_PULLUP;
	GPIO_InitA.Speed =  GPIO_SPEED_FREQ_HIGH;
	//GPIO_Init.Alternate = ;
	
	
	GPIO_InitC.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitC.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitC.Pull = GPIO_PULLUP;
	GPIO_InitC.Speed =  GPIO_SPEED_FREQ_HIGH;
	//GPIO_Init.Alternate = ;
	
	HAL_GPIO_Init(GPIOA, &GPIO_InitA);
	HAL_GPIO_Init(GPIOC, &GPIO_InitC);
	
	
}
float getTemperature() {
	//HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout)

	float VSENSE, Avg_Slope, V25, temp;	
	
	VSENSE= HAL_ADC_PollForConversion(&ADC1_Handle, 200);
	HAL_ADC_GetValue(&ADC1_Handle); //gets the temperature voltage value
	
	Avg_Slope = (125+40)/((3.6-1.8)*1000); //micro celcius/V
	V25 = Avg_Slope*25;
	
	//uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc); //error handling
	
	// Temperature (in °C) = {(VSENSE – V25) / Avg_Slope} + 25
	
	return temp = ((VSENSE - V25)/Avg_Slope) + 25;
}

int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
	config_ADC_temp();
	
	// Initialize and start ADC1

	HAL_ADC_Init(&ADC1_Handle);

	
//	HAL_SYSTICK_Config(SystemCoreClock/50); //tick every 20 ms //Input value must be less than 24 bits
	
	// Insert our system here
 
	while(1) {
		float temp = getTemperature();
		HAL_ADC_Start(&ADC1_Handle);	
		printf("Tempertaure: %f \n",temp);
	}
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
