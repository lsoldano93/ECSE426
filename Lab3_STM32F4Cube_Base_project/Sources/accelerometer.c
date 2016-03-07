#include "accelerometer.h"

accelerometer_values accel;

// Takes in the raw inputs and outputs a more exact value
void update_accel_values(float Ax, float Ay, float Az) {
	accel.x = Ax*ACC11 + Ay*ACC12 + Az*ACC13 + ACC10;
	accel.y = Ax*ACC21 + Ay*ACC22 + Az*ACC23 + ACC20;
	accel.z = Ax*ACC31 + Ay*ACC32 + Az*ACC33 + ACC30;
}

float calc_pitch_angle(void){
		return DEGREES(atan(accel.y/sqrt((accel.x*accel.x)+(accel.z*accel.z))));

}

float calc_roll_angle(void){	
	return DEGREES(atan(accel.x/sqrt((accel.y*accel.y)+(accel.z*accel.z))));
}

float calc_yaw_angle(void){
	return DEGREES(acos(accel.z/1000.0));
}


void init_accelerometer(void) {
	
	//Configure LIS3DSH accelermoter sensor
	LIS3DSH_InitTypeDef init;
	LIS3DSH_DRYInterruptConfigTypeDef init_it;
	uint8_t data_ready;
	GPIO_InitTypeDef GPIO_InitE;
	
	init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_25;  	// Active mode with data rate 100HZ
	init.Axes_Enable = LIS3DSH_XYZ_ENABLE; 										// Enable all axes
	init.Continous_Update = LIS3DSH_ContinousUpdate_Enabled;  // Enable continuous update
	init.AA_Filter_BW = LIS3DSH_AA_BW_50; 										// Not sure about this one, BW = ODR/2
	init.Full_Scale = LIS3DSH_FULLSCALE_2; 										// Use full scale range of 2g
	init.Self_Test = LIS3DSH_SELFTEST_NORMAL; 								// Keep this set to normal
	LIS3DSH_Init(&init);


	// Accelerometer interrupt configuration
  init_it.Dataready_Interrupt = LIS3DSH_DATA_READY_INTERRUPT_ENABLED;
  init_it.Interrupt_signal = LIS3DSH_ACTIVE_HIGH_INTERRUPT_SIGNAL;                  
  init_it.Interrupt_type = LIS3DSH_INTERRUPT_REQUEST_PULSED;      
	
		// Set GPIO pin PE0 as interrupt input for acceleromter data 
	// set PE0 as the interupt for the accelorometer

	
	// Enable clocks for ports E
	__HAL_RCC_GPIOE_CLK_ENABLE();

	// Give initialization values for GPIO E pin sets
	GPIO_InitE.Pin = GPIO_PIN_0 ;
	GPIO_InitE.Mode = GPIO_MODE_IT_RISING; // TODO: Pin needs to be set in interrupt mode, not sure if supposed to use rising/falling edge
	GPIO_InitE.Pull = GPIO_PULLDOWN;
	GPIO_InitE.Speed =  GPIO_SPEED_FREQ_HIGH;
	//GPIO_InitE.Alternate =  TODO Do we need this one and if so what are the parameters we can set it to? 
	
	// Initialize GPIO PE0
	HAL_GPIO_Init(GPIOE, &GPIO_InitE);	
	
	//enable external interrupt line 0
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0); //set group and sub priority
	HAL_NVIC_ClearPendingIRQ(EXTI0_IRQn);
	
	
	LIS3DSH_DataReadyInterruptConfig(&init_it); 

}