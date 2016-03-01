#include "stm32f4xx_hal.h"
#include "accelerometer.h"

#define PI 3.14159265358

//need to tun ACC10, ACC20, ACC30
#define ACC11 1 
#define ACC12 0 
#define ACC13 0
#define ACC10 0 //Add to x
#define ACC21 0
#define ACC22 1
#define ACC23 0
#define ACC20 0 //Add to y
#define ACC31 0
#define ACC32 0
#define ACC33 1
#define ACC30 70 //Add to the z 

#define DEGREES(x) (180.0*x/PI)

typedef struct {
	float x; 
	float y; 
	float z;
} accelerometer_values;

accelerometer_values accel;

//this function initialize the interrupts for the accelerometer
void init_interrupts() {
	
	// Set GPIO pin PE0 as interrupt input for acceleromter data 
	// set PE0 as the interupt for the accelorometer
	GPIO_InitTypeDef GPIO_InitE;
	
	// Enable clocks for ports E
	__HAL_RCC_GPIOE_CLK_ENABLE();

	// Give initialization values for GPIO E pin sets
	GPIO_InitE.Pin = GPIO_PIN_0 ;
	GPIO_InitE.Mode = GPIO_MODE_IT_RISING; //pin needs to be set in interrupt mode, not sure if to use rising/falling edge
	GPIO_InitE.Pull = GPIO_PULLDOWN;
	GPIO_InitE.Speed =  GPIO_SPEED_FREQ_VERY_HIGH;
	
	// Initialize GPIO PE0
	HAL_GPIO_Init(GPIOE, &GPIO_InitE);	
	
	//enable external interrupt line 0
	//SYSCFG_EXTICR1_EXTI0_PE is pin PE0
	HAL_GPIO_EXTI_IRQHandler(SYSCFG_EXTICR1_EXTI0_PE);
	
	// set up NVIC
	// PE0 is connected to EXTI_Line0, which has EXTI0_IRQn vector 
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_0); //not sure if we need this one
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0); //set group and sub priority
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

//takes in the raw inputs and outputs a more exact value
void update_accel_values(float Ax, float Ay, float Az) {
	accel.x = Ax*ACC11 + Ay*ACC12 + Az*ACC13 + ACC10;
	accel.y = Ax*ACC21 + Ay*ACC22 + Az*ACC23 + ACC20;
	accel.z = Ax*ACC31 + Ay*ACC32 + Az*ACC33 + ACC30;
}

float calc_pitch_angle(void){
	return DEGREES(atan(accel.x/sqrt((accel.y*accel.y)+(accel.z*accel.z))));
}

float calc_roll_angle(void){	
	return DEGREES(atan(accel.y/sqrt((accel.x*accel.x)+(accel.z*accel.z))));
}

float calc_yaw_angle(void){
	return DEGREES(acos(accel.z/1000.0));
}


void init_accelerometer() {
	//Configure LIS3DSH accelermoter sensor
	LIS3DSH_InitTypeDef init;
	
	init.Power_Mode_Output_DataRate = LIS3DSH_DATARATE_100; //active mode with data rate 100HZ
	init.Axes_Enable = LIS3DSH_XYZ_ENABLE; //enable all axes
	init.Continous_Update = LIS3DSH_ContinousUpdate_Disabled; //disable continuous update
	init.AA_Filter_BW = LIS3DSH_AA_BW_50; //not sure about this one, BW = ODR/2
	init.Full_Scale = LIS3DSH_FULLSCALE_2; //use full scale range of 2g
	init.Self_Test = LIS3DSH_SELFTEST_NORMAL; //keep this set to normal
	LIS3DSH_Init(&init);
	
}