#include <stdio.h>
#include "keypad.h"

/**
   * @brief Initializes keypad for proper operation by configuring GPIO pins as input
   */
void init_keypad(void) {
	
	//initialize GPIO pins for the keypad
	GPIO_InitTypeDef GPIO_InitD;
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
		// Give initialization values for GPIO D pin sets
	GPIO_InitD.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	
	GPIO_InitD.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitD.Pull = GPIO_PULLDOWN;
	GPIO_InitD.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitD);

}

/**
   * @brief Reports column of key that was pressed
   * @retval Int of column activated
   */
uint8_t get_column(void) {
	
	// Row selection determined by active low (TODO find pins)
	if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1)) return 0;
	else if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_2)) return 1;
	else if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_6)) return 2;
	else return 9;
	
}

/**
   * @brief Reports row of key that was pressed
   * @retval Int of row activated
   */
uint8_t get_row(void) {
	
	// Row selection determined by active low (TODO find pins)
	if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_7)) return 0;
	else if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8)) return 1;
	else if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9)) return 2;
	else if(!HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10)) return 3;
	else return 9;

}

int get_key(void) {
	
	uint8_t row, column;
	
	// Interupt handling, and debouncing;
	if((row = get_row()) == 9) return -1;
	if((column = get_column()) == 9) return -1;
	
	return keypad_map[row][column];		
}

// TODO: Debounce
int handle_key_press(void) {

	//if(currentKey != prevKey){
		//insert code into here
		//add delay
	//}


	int key;
	int i = 0; 
	int angle = 0;
	
	// If first key pressed is not digit, press was not important
	if((key = get_key()) == 9 || key == 0 || key == 11 || key == 12) return -1;
	
	angle = key;
	
	while(1){
		
		// Wait for key press to be relavant
		while((key = get_key()) == 9 || key == 11);
		
		if (key != 12) angle = angle * 10 + key;
		else break;
		
		if((i += 1) == 2) break;
	}
	
	while((key = get_key()) != 12);
	
	return angle;
	
}


