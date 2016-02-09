/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : A program which showcases ADC and TIM3 under the new firmware
	                       The ADC
	* Author						 : Ashraf Suyyagh
	* Version            : 1.0.0
	* Date							 : January 14th, 2016
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_conf.h"
#include "supporting_functions.h"

/* Initialize -------------------------------------------------------------------*/
/* Code referenced from: http s://my.st.com/public/STe2ecommunities/mcu/Lists/STM32Java/Flat.aspx?RootFolder=https%3a%2f%2fmy%2est%2ecom%2fpublic%2fSTe2ecommunities%2fmcu%2fLists%2fSTM32Java%2fSTM32F3%20internal%20temp%20sensor&FolderCTID=0x01200200770978C69A1141439FE559EB459D758000F9A0E3A95BA69146A17C2E80209ADC21&currentviews=2303 */
void config_ADC_temp() {
	
	// Initialize struct for ADC
	ADC_InitTypeDef ADC_Init_S; 
	
	// Common init struct defined for ADC
	ADC_CommonInitTypeDef ADC_Common_Init_S;
	
	// Reset all ADCs to their default values.
	ADC_DeInit(); 
	
	// Enable ADC1 clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	// Set ADC to operate in independent mode
	ADC_Common_Init_S.ADC_Mode= ADC_Mode_Independent;
	
	// Select freq of ADC's clock - Div2 means clock counts 0 & 1 each cycle
	ADC_Common_Init_S.ADC_Prescaler = ADC_Prescaler_Div2;
	
	// Disable DMA access mode for multiple ADCs
	ADC_Common_Init_S.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; 
	
	// Configure delay between two sampling phases
	ADC_Common_Init_S.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; 
	
	// Give ADC peripherals values according to parameters previously specified
	ADC_CommonInit(&ADC_Common_Init_S);
	
	// ADC resolution set to 12 bits
	ADC_Init_S.ADC_Resolution = ADC_Resolution_12b; 
	
	// Set conversion to perform in single channel mode
	ADC_Init_S.ADC_ScanConvMode= DISABLE; 
	
	// Set ADC to convert value only when signaled to do so
	ADC_Init_S.ADC_ContinuousConvMode= DISABLE; 
	
	// Set no external conversion trigger edge
	ADC_Init_S.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; 
	
	// Allign ADC data on right, data saved starting at LSB
	ADC_Init_S.ADC_DataAlign = ADC_DataAlign_Right; 
	
	// Set regular channel sequence length to 1
	ADC_Init_S.ADC_NbrOfConversion = 1; 
	
	// Enable temperature sensor
	ADC_TempSensorVrefintCmd(ENABLE);
	
	// Enable ADC1
	ADC_Cmd(ADC1, ENABLE); 
	
	// Enable ADC1 and connect to 16th channel (temperature sensor), while giving it first rank in sequencer and a sample time of 480 cycles
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); 
	
	// Initialize ADC1 to values of previously defined struct
	ADC_Init(ADC1, &ADC_Init_S); 
	
	
}
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config	(void);


int main(void)
{
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
	
	
	// Insert our system here
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDOn(LED3);
//	while (1){
//		
//		
//		
//	}
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
