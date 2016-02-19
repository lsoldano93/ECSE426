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

#define ALARM_THRESHOLD_TEMP 35
#define ARRAY_LENGTH 1
#define kalmanQ 0.2
#define kalmanR 20.1
#define kalmanX 29.5			// Standard base room temp reading of sensor
#define kalmanP 0.0
#define kalmanK 0.0

/** 
  * @brief Structure for the Kalman filter  
  */ 
typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

/* Private variables ---------------------------------------------------------*/

ADC_HandleTypeDef ADC1_Handle;
int timer(__IO uint32_t time);
float tempValue;

/* Public variables ----------------------------------------------------------*/
//extern __IO uint32_t TimmingDelay;
//extern __IO uint32_t TimerDelay;

/* Private function prototypes -----------------------------------------------*/

/**  Assembly Kalmann filter function
   * @brief  Filters values to remove noisy fluctuations
	 * @param  Input value of measurements, array to output to, length of arrays, and kalman parameter struct
   * @retval Returns updated output array **/
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);

/**  ADC Configuration
   * @brief  Configures ADC1 Channel 16 so that temperature values can be read
   */
void ADC_Config(void);

/**  GPIO Configuration
   * @brief  Configures GPIO pins for proper system outputs to match to physical circuit
   */
void GPIO_Config(void);

/**  Get temperature
   * @brief  Obtains temperature voltage readout from ADC1 Channel 16
   * @retval Temperature float in celcius
   */
float getTemp(void);

/**  Function to display temperature
   * @brief  Breaks temperature into individual digits for display then calls
   *         lightDigit function to have that digit displayed
   * @param  Temperature to be dispalyed
   */
void displayTemp(float temp);

/**  Light Digit
   * @brief  Method for lighting a digit on the 7 segment display, where segment
   *         control has already been assigned to a specific digit in the display
   * @param  Digit to be displayed
   */
void lightDigit(int digit);

/**  Temperature alarm function
   * @brief  If alarm is triggered, light LEDs in sequential circular pattern
   * @param  None
   * @retval None
   */
void tempAlarm();

/**  Delay
   * @brief  Increments a timer tick to allow the use of delays
   * @param  System clock timer value
   * @retval None
   */
void Delay(__IO uint32_t time);

/**  Timer
   * @brief  Increments a timer tick to allow the use of delays
   * @param  System clock timer value
   * @retval None
   */
void SystemClock_Config(void);

/* Initialize -------------------------------------------------------------------*/

int ticks = 0;
int alarmCount = 0;

/* Private Functions ------------------------------------------------------------*/

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


/**  GPIO Configuration
   * @brief  Configures GPIO pins for proper system outputs to match to physical circuit
   * @param  None
   * @retval None
   */
void GPIO_Config() {
	
	// WARNING: DO NOT USE PORT A13, PORT A14, or PORT B3
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:CCD123, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}
	   GPIO Pin Mapping
		 Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
		 DP Ctrl 	     {DP:PC6}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14 
	Alarm LED : PD13
	*/
	
	GPIO_InitTypeDef GPIO_InitB, GPIO_InitC, GPIO_InitD;
	
	// Enable clocks for ports B & C & D
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	// Give initialization values for GPIO B pin sets
	GPIO_InitB.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitB.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitB.Pull = GPIO_PULLUP;
	GPIO_InitB.Speed =  GPIO_SPEED_FREQ_HIGH;
	
	// Give initialization values for GPIO C pin sets
	GPIO_InitC.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6;
	GPIO_InitC.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitC.Pull = GPIO_PULLUP;
	GPIO_InitC.Speed =  GPIO_SPEED_FREQ_HIGH;
	
	// Give initialization values for GPIO D pin sets
	GPIO_InitD.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitD.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitD.Pull = GPIO_PULLUP;
	GPIO_InitD.Speed =  GPIO_SPEED_FREQ_HIGH;
	
	// Initialize all GPIO pin sets
	HAL_GPIO_Init(GPIOB, &GPIO_InitB);
	HAL_GPIO_Init(GPIOC, &GPIO_InitC);
	HAL_GPIO_Init(GPIOD, &GPIO_InitD);
	
	__HAL_ADC_ENABLE(&ADC1_Handle);

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


/**  Function to display temperature
   * @brief  Breaks temperature into individual digits for display then calls
   *         lightDigit function to have that digit displayed
   * @param  Temperature to be dispalyed
   * @retval None
   */
void displayTemp(float temp){
	
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:CCD123, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}	
	   GPIO Pin Mapping
	   Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
		 DP Ctrl 	     {DP:PC6}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14 */
	
	int displayValues, i;
	int tempValue[4];
	
	// Determine how many values to display and what each individual value is
	// Four digits to display (format xyz.a)
	if (temp >= 100.0){
		displayValues = 4;
		tempValue[0] = temp/100;
		tempValue[1] = temp/10 - tempValue[0]*10;
		tempValue[2] = temp - tempValue[0]*100 - tempValue[1]*10;
		tempValue[3] = (temp*10 - tempValue[0]*1000 - tempValue[1]*100 - tempValue[2]*10);
	
	}
	else {
		
		// Three digits to display (format xy.z)
		if (temp >= 10.0){
				displayValues = 3;
				tempValue[1] = temp/10;
				tempValue[2] = temp - tempValue[1]*10;
				tempValue[3] = (temp*10 - tempValue[1]*100 - tempValue[2]*10);	
		}
		
		// Two digits to display (format x.y or 0.x)
		else {
				displayValues = 2;
				tempValue[2] = temp;
				tempValue[3] = (temp*10 - tempValue[2]*10);
		}
	}
	
	for(i=4; i>0; i--) {
		
		switch(i) {
			case 4:
				// Enable changes for first digit display
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);   // Digit 1
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
				break;
			case 3:
				// Enable changes for second digit display
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Digit 1
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);   // Digit 2
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
				break;
			case 2:
				// Enable changes for third digit display
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Digit 1
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);   // Digit 3
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET); // Digit 4
				break;
			case 1:
				// Enable changes for fourth digit display
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); // Digit 1
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); // Digit 2
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET); // Digit 3
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);   // Digit 4
				break;
	
			default:
				break;
		} 
		
		// Light proper number on display, lighting 10 indicates blank display value
		if (i > displayValues) {
			lightDigit(10);
		}
		else {
			// Set decimal point on third value
			if (i == 2) HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
			
			lightDigit(tempValue[4-i]);	
			
		}
		
		Delay(1);
	}	
	
}
	

/**  Light Digit
   * @brief  Method for lighting a digit on the 7 segment display, where segment
   *         control has already been assigned to a specific digit in the display
   * @param  Digit to be displayed
   * @retval None
   */
void lightDigit(int digit) {
	
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:CCD123, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}	
	   GPIO Pin Mapping
	   Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
		 DP Ctrl 	     {DP:PC6}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14 */
	
	switch(digit) {
			case 0:
				// Zeroth state, display number zero
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
				break;
			
			case 1:
				// First state, display number one
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
				break;

			case 2:
				// Second state, display number two
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 3:
				// Third state, display number three
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 4:
				// Fourth state, display number four
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g			
				break;
			
			case 5:
				// Fifth state, display number five
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 6:
				// Sixth state, display number six
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;

			case 7:
				// Seventh state, display number seven
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g
				break;
			
			case 8:
				// Eighth state, display number eight
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g
				break;
			
			case 9:
				// Ninth state, display number nine
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET); //g			
				break;
			
			case 10:
				// Null state, display blank digit
				//printf("Case %d", digit);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); //a
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET); //b
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET); //c
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET); //d
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //e
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); //f
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //g		
				break;
				
			default:
				break;
		}
	
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


/**  Delay
   * @brief  Increments a timer tick to allow the use of delays
   * @param  System clock timer value
   * @retval None
   */
void Delay(__IO uint32_t time)
{
	
  TimmingDelay = time;
  while(TimmingDelay !=0);
	
}     


/**  Timer
   * @brief  Increments a timer tick to allow the use of delays
   * @param  System clock timer value
   * @retval None
   */
int timer(__IO uint32_t time) {
	TimerDelay = time;

	while(TimerDelay !=0);
	return 1;
}


/**  Main Method
   * @brief  Initializes the Kalman filter, calls configuration functions for
   *         the system clock, ADC, and GPIO pins, then enters a while loop
   *         that updates temperature + display, and checks alarm conditions
   * @param  None
   * @retval None
   */
int main(void)
{
	
	int counter;
	float temp, temps[ARRAY_LENGTH], output[ARRAY_LENGTH];
	kalman_t kalmanFilter;
	
	// Give kalmanFilter, temp, ticks initial values
	counter = 0;
	temp = 0;
	ticks = 0;
	alarmCount = 0;
	
	kalmanFilter.q = kalmanQ;
	kalmanFilter.r = kalmanR;
	kalmanFilter.x = kalmanX;
	kalmanFilter.p = kalmanP;
	kalmanFilter.k = kalmanK;
	
  /* MCU Configuration----------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
	
  /* Configure the system clock */
  SystemClock_Config();
	ADC_Config();
	GPIO_Config();

	while(1) {
		
		// Wait for ticks flag to be set high by SysTick
		if (ticks){

			counter++;
			
			if (counter == 10){
				// Ping ADC for reading and obtain temperature
				temps[0] = getTemp();
					
				// Pass values to assembly filter and set last filtered output as current temp
				Kalmanfilter_asm(temps, output, ARRAY_LENGTH, &kalmanFilter);
				temp = output[0];
				
				printf("Temperature_Filter = %f, Temperature_Reading = %f16\n", temp, temps[0]);
				counter = 0;
			}
			
			// Set global tick variable to 0 and wait for SysTick to reset flag
			ticks = 0;
		}
		
		// Display temperature
		displayTemp(temp);
		
		// Trigger alarm if proper time has passed and temperature greater than threshold
		if (timer(5) == 1){
			if(temp >= ALARM_THRESHOLD_TEMP) {
				tempAlarm();
			}
			else {
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
			}
	}
}
	
return NULL;

}


/**  System Clock Configuration
   * @brief  Configures the system clock for proper operation
   * @param  None
   * @retval None
   */
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
