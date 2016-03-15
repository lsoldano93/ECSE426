
#include "Thread_TempSensor.h"

float temperatureValue;

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef ADC1_Handle;
osThreadId tid_Thread_TempSensor;   
kalman_t kalman_temperature;
const void* temperatureMutexPtr;

osThreadDef(Thread_TempSensor, osPriorityNormal, 1, NULL); // TODO: Can we have multiple priorities of 0

/**  Initiates temperature sensor thread
   * @brief  Builds thread and starts it
   * @retval Temperature float in celcius
   */
int start_Thread_TempSensor (void) {

  tid_Thread_TempSensor = osThreadCreate(osThread(Thread_TempSensor ), NULL); // Start LED_Thread
  if (!tid_Thread_TempSensor){
		printf("Error starting temperature sensor thread!");
		return(-1); 
	}
  return(0);
}

/**  Get temperature
   * @brief  Obtains temperature voltage readout from ADC1 Channel 16
   * @param  None
   * @retval Temperature float in celcius
   */
void Thread_TempSensor (void const *argument){
	
	while(1){
			//use tim3 timer instead
			//need to use osWait
//			osDelay(100);
			Delay(100);
			updateTemp();
	}
}
	
/**  Get temperature
   * @brief  Obtains temperature voltage readout from ADC1 Channel 16
   * @param  None
   * @retval Temperature float in celcius
   */
void updateTemp(void) {

	float VSENSE;	
	
	// Obtain temperature voltage value from ADC
	//need to get poll working
	HAL_ADC_PollForConversion(&ADC1_Handle,10);
	VSENSE = HAL_ADC_GetValue(&ADC1_Handle); 
	
	// Filter raw temperature sensor values
	//Kalmanfilter_asm(&VSENSE, &VSENSE, 1, &kalman_temperature);
	
	/* Obtain permission for access to tempValue and then Supdate
	   ---------------------------------------------------------
		 ADC 3.0 Volts per 2^12 steps (12 bit resolution in configuration)
	   Voltage at 25C is 760mV
	   Avg slop is 25mV/1C 
	   --------------------------------------------------------- */
	osMutexWait(temperatureMutex, 10);
	temperatureValue  = (VSENSE*(3.0f/ 4096.0f) - 0.76f)/0.025f + 25.0f;	
	printf("Temperature Value: %f\n", temperatureValue);
	osMutexRelease(temperatureMutex);	

	return;
	
}


/**  Uses timer 3 to generate delay for display
   * @brief  Allows for software delay to be used for display purposes **/
void Delay(uint32_t time)
{
	
  TimmingDelay = time;
  while(TimmingDelay !=0);
	
}  

/**  ADC Configuration
   * @brief  Configures ADC1 Channel 16 so that temperature values can be read
   */
void ADC_config(void) {

	ADC_ChannelConfTypeDef ADC1_ch16;
	
	// Enable ADC clock
	__ADC1_CLK_ENABLE();
	
	// Initialize values for ADC1 handle type def
	ADC1_Handle.Instance = ADC1;
	ADC1_Handle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	ADC1_Handle.Init.Resolution = ADC_RESOLUTION_12B;    									 
	ADC1_Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;         						
	ADC1_Handle.Init.ScanConvMode = DISABLE;           
	ADC1_Handle.Init.EOCSelection = ENABLE;           			
	ADC1_Handle.Init.ContinuousConvMode = DISABLE;      //
	ADC1_Handle.Init.DMAContinuousRequests = DISABLE;  
	ADC1_Handle.Init.NbrOfConversion = 1;       													
	ADC1_Handle.Init.DiscontinuousConvMode = DISABLE;  
	ADC1_Handle.Init.NbrOfDiscConversion = 0;    
	ADC1_Handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;       //
	ADC1_Handle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;  //
	
	// Initialize values for temperature sensor (Temperature analog channel is Ch16 of ADC1)
	ADC1_ch16.Channel = ADC_CHANNEL_16;
	ADC1_ch16.Rank = 1;
	ADC1_ch16.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	ADC1_ch16.Offset = 0;
	
	// Initialize clock with error handling
	HAL_ADC_Init(&ADC1_Handle);
	
	// Configure temperature sensor peripheral 
	HAL_ADC_ConfigChannel(&ADC1_Handle, &ADC1_ch16);
	HAL_ADC_Start(&ADC1_Handle);
	
	// Initialize temperature sensor mutex
	temperatureMutex = osMutexCreate(temperatureMutexPtr);
	
	// Allot values to the kalman filtration struct for the temperature sensor
	kalman_temperature.q = tempKalman_q;
	kalman_temperature.r = tempKalman_r;
	kalman_temperature.x = tempKalman_x;
	kalman_temperature.p = tempKalman_p;
	kalman_temperature.k = tempKalman_k;
	
}

