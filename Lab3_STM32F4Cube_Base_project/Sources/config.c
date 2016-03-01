#include "sys_config.h"
#include "stm32f4xx_hal.h"



void GPIO_Config() {
	
	
	GPIO_InitTypeDef GPIO_InitE;
	
	// Enable clocks for ports E
	__HAL_RCC_GPIOE_CLK_ENABLE();

	
	// Give initialization values for GPIO E pin sets
	GPIO_InitE.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 ;
	GPIO_InitE.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitE.Pull = GPIO_PULLUP;
	GPIO_InitE.Speed =  GPIO_SPEED_FREQ_HIGH;
	
	// Initialize all GPIO pin sets

	HAL_GPIO_Init(GPIOE, &GPIO_InitE);
	
	
	
	
}

void MEMS_Config() {

	
}



