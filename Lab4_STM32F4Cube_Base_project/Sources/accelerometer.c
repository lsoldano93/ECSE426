/* Includes ------------------------------------------------------------------*/
#include "accelerometer.h"

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

osThreadDef(Thread_Accelerometer, osPriorityNormal, 1, NULL); // TODO: Can we have multiple priorities of 0

/**  Initiates temperature sensor thread
   * @brief  Builds thread and starts it
   * @retval Temperature float in celcius
   */
	 
int start_Thread_Accelerometer (void) {
  tid_Thread_Accelerometer = osThreadCreate(osThread(Thread_Accelerometer ), NULL); // Start acclerometer thread
  if (!tid_Thread_Accelerometer){
		printf("Error starting acclerometer thread!");
		return(-1); 
	}
  return(0);
}

void Thread_Accelerometer (void const *argument){
	
	while(1){
		accelerometer_mode();
	}
}

void accelerometer_mode(void) {
	//add in mutex wait here
	update_accel_values(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]);
			
	roll = calc_roll_angle();
	pitch = calc_pitch_angle();
			
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

void config_accelerometer_kalman(void) {
	
	//Configure kalman filter for x
	kalmanX.q = 0.02;
	kalmanX.r = 4.5;
	kalmanX.x = 90.0;
	kalmanX.p = 0.0;
	kalmanX.k = 0.0;
		
	//Configure kalman filter for y
	kalmanY.q = 0.02;
	kalmanY.r = 4.5;
	kalmanY.x = 90.0;
	kalmanY.p = 0.0;
	kalmanY.k = 0.0;
	
	
	//Configure kalman filter for z
	kalmanZ.q = 0.02;
	kalmanZ.r = 4.5;
	kalmanZ.x = 90.0;
	kalmanZ.p = 0.0;
	kalmanZ.k = 0.0;
	
}



/**  Accelerometer bread and butter
   * @brief  Updates x, y, z parameters of accelerometer by reading from MEMs device
	 * @param  Locations where updated values will be stored **/
void update_accel_values(float Ax, float Ay, float Az) {
	accel.x = Ax*ACC11 + Ay*ACC12 + Az*ACC13 + ACC10;
	accel.y = Ax*ACC21 + Ay*ACC22 + Az*ACC23 + ACC20;
	accel.z = Ax*ACC31 + Ay*ACC32 + Az*ACC33 + ACC30;
	
	
	Kalmanfilter_asm(&accel.x, &accel.x, 1, &kalmanX);	
	Kalmanfilter_asm(&accel.y, &accel.y, 1, &kalmanY);	
	Kalmanfilter_asm(&accel.z, &accel.z, 1, &kalmanZ);	
}

/**  Calculates pitch angle
   * @brief  Calculates pitch angle
   * @retval Returns pitch angle **/
float calc_pitch_angle(void){
	return 90.0 - DEGREES(atan(accel.y/sqrt((accel.x*accel.x)+(accel.z*accel.z))));

}

/**  Calculates roll angle
   * @brief  Calculates roll angle
   * @retval Returns roll angle **/
float calc_roll_angle(void){	
	return 90.0 - DEGREES(atan(accel.x/sqrt((accel.y*accel.y)+(accel.z*accel.z))));
}

/**  Calculates yaw angle
   * @brief  Calculates yaw angle
   * @retval Returns yaw angle **/
float calc_yaw_angle(void){
	return 90.0 - DEGREES(acos(accel.z/1000.0));
}

/**  Initialize accelerometer
   * @brief  Initializes accelerometer for use **/
void init_accelerometer(void) {
	
	//Configure LIS3DSH accelermoter sensor
	LIS3DSH_InitTypeDef init;
	LIS3DSH_DRYInterruptConfigTypeDef init_it;
	GPIO_InitTypeDef GPIO_InitE;
	
	init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;  	// Active mode with data rate 100HZ
	init.Axes_Enable = LIS3DSH_XYZ_ENABLE; 										// Enable all axes
	init.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;  // Enable continuous update
	init.AA_Filter_BW = LIS3DSH_AA_BW_50; 										// TODO: Not sure about this one, BW = ODR/2
	init.Full_Scale = LIS3DSH_FULLSCALE_2; 										// Use full scale range of 2g
	init.Self_Test = LIS3DSH_SELFTEST_NORMAL; 								// Keep this set to normal
	LIS3DSH_Init(&init);

	// Accelerometer interrupt configuration
  init_it.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
  init_it.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;                  
  init_it.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;      
	
	/* Set PE0 as the interupt for the accelorometer
	   Enable clocks for ports E */
	__HAL_RCC_GPIOE_CLK_ENABLE();

	// Give initialization values for GPIO E pin sets
	GPIO_InitE.Pin = GPIO_PIN_0 ;
	GPIO_InitE.Mode = GPIO_MODE_IT_RISING; 
	GPIO_InitE.Pull = GPIO_PULLDOWN;
	GPIO_InitE.Speed =  GPIO_SPEED_FREQ_HIGH;
	
	// Initialize GPIO PE0
	HAL_GPIO_Init(GPIOE, &GPIO_InitE);	
	
	// Enable external interrupt line 0
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
	LIS3DSH_DataReadyInterruptConfig(&init_it); 
	
	//initialize kalman filters for the accelerometer
	config_accelerometer_kalman();

}