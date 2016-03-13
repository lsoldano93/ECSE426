#include "temp_sensor.h"


/* Private variables ---------------------------------------------------------*/

ADC_HandleTypeDef ADC1_Handle;
int timer(__IO uint32_t time);
float tempValue;

/**  ADC Configuration
   * @brief  Configures ADC1 Channel 16 so that temperature values can be read
   * @param  None
   * @retval None
   */
void ADC_Config(void) {


	ADC_ChannelConfTypeDef ADC1_ch16;
	
	// Initialize values for ADC1 handle type def
	ADC1_Handle.Instance = ADC1;
	ADC1_Handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	ADC1_Handle.Init.Resolution = ADC_RESOLUTION_12B;    									// 12 bit resolution      
	ADC1_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;         						// Data alligns to right
	ADC1_Handle.Init.ScanConvMode = DISABLE;           
	ADC1_Handle.Init.EOCSelection = DISABLE;           				// Single conversion mode
	ADC1_Handle.Init.ContinuousConvMode = ENABLE;     
	ADC1_Handle.Init.DMAContinuousRequests = DISABLE;  
	ADC1_Handle.Init.NbrOfConversion = 1;       													// Single conversion per call
	ADC1_Handle.Init.DiscontinuousConvMode = DISABLE;  
	ADC1_Handle.Init.NbrOfDiscConversion = 0;    
	ADC1_Handle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;       
	ADC1_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  
	
	// Initialize values for temperature sensor (Temperature analog channel is Ch16 of ADC1)
	ADC1_ch16.Channel = ADC_CHANNEL_16;
	ADC1_ch16.Rank = 1;
	ADC1_ch16.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	ADC1_ch16.Offset = 0;
		
	// Enable ADC clock
	__ADC1_CLK_ENABLE();
	
	// Initialize clock with error handling
	if (HAL_ADC_Init(&ADC1_Handle) != HAL_OK) Error_Handler(ADC_INIT_FAIL);
	
// Configure temperature sensor peripheral 
	HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC1_ch16);
	HAL_ADC_Start(&ADC1_Handle);
	
}

/**  Get temperature
   * @brief  Obtains temperature voltage readout from ADC1 Channel 16
   * @param  None
   * @retval Temperature float in celcius
   */
float getTemp() {

	float VSENSE, temp;	
	
	// Obtain temperature voltage value from ADC
	VSENSE = HAL_ADC_GetValue(&ADC1_Handle); 
	
	/* ADC 3.3 Volts per 2^12 steps (12 bit resolution in configuration)
	   Voltage at 25C is 760mV
	   Avg slop is 25mV/1C */
	tempValue  = (VSENSE*(3.3f/ 4096.0f) - (float)0.76 )/(float)0.025 + 25.0;			

	return tempValue;
	
}

/**  Temperature alarm function
   * @brief  If alarm is triggered, light LEDs in sequential circular pattern
   * @param  None
   * @retval None
   */
void tempAlarm() {
	
	// Green light on
	if (alarmCount < 500){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	}
	else {
		
		// Orange light on
		if (alarmCount < 1000){
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		}
		else{
			
			// Red light on
			if (alarmCount < 1500){
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
			}
			
			// Blue light on
			else{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
			}
		}
	}

}