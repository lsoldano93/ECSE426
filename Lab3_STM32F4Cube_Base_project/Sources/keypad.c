#include <stdio.h>
#include "keypad.h"

/* Private functions ---------------------------------------------------------*/

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
   * @brief Reports whether or not key is still active low after DEBOUNCE_DELAY ms
	 * @param Row and col GPIO pins associated with D-set where debounce should be checked
   * @retval 1 if proper value read, 0 if effects due to bounce detected
   */
uint8_t debounce(uint16_t COL_PIN, uint16_t ROW_PIN){
	
	uint8_t adjustedDelay = 0;
	
	/* Get value of timer counter, if on back end of counter set bool to true
		 for calculations to work properly */
	uint16_t lastDebounceTime = tim3_ticks;
	if (lastDebounceTime + DEBOUNCE_DELAY > TIM3_BOUND) adjustedDelay = DEBOUNCE_DELAY + lastDebounceTime - TIM3_BOUND;
	
	while(1) {
		
		if(adjustedDelay == 0){
			// Ensure reading is still active low after set debounce time
			if ((tim3_ticks - lastDebounceTime) > DEBOUNCE_DELAY) {
				if (HAL_GPIO_ReadPin(GPIOD, COL_PIN) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOD, ROW_PIN) == GPIO_PIN_RESET) return 1;
				else return 0;
			}
		}
		else {
			// Ensure reading is still active low after set debounce time
			if (tim3_ticks > DEBOUNCE_DELAY && tim3_ticks < lastDebounceTime) {
				if (HAL_GPIO_ReadPin(GPIOD, COL_PIN) == GPIO_PIN_RESET && HAL_GPIO_ReadPin(GPIOD, ROW_PIN) == GPIO_PIN_RESET) return 1;
				else return 0;
			}
		}
		
	}
}


/**
   * @brief Reports column of key that was pressed
   * @retval Int of column activated
   */
uint8_t get_column(void) {
	
	// Row selection determined by active low 
	if(!HAL_GPIO_ReadPin(GPIOD, col_pinmap[0])) return 0;
	else if(!HAL_GPIO_ReadPin(GPIOD, col_pinmap[1])) return 1;
	else if(!HAL_GPIO_ReadPin(GPIOD, col_pinmap[2])) return 2;
	else return 9;
	
}


/**
   * @brief Reports row of key that was pressed
   * @retval Int of row activated
   */
uint8_t get_row(void) {
	
	// Row selection determined by active low (TODO find pins)
	if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[0])) return 0;
	else if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[1])) return 1;
	else if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[2])) return 2;
	else if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[3])) return 3;
	else return 9;

}


/**
   * @brief Determines row and column of key press then returns value from map
   * @retval Returns exact key pressed
   */
int get_key(void) {
	
	uint8_t row, column;
	
	// Interupt handling, and debouncing;
	if((row = get_row()) == 9) return -1;
	if((column = get_column()) == 9) return -1;
	if(!debounce(col_pinmap[column], row_pinmap[row])) return -1;
	
	return keypad_map[row][column];		
}


/**
   * @brief Reports action on keypad
   * @retval -1 if no key pressed yet, angle value if they have
   */
int handle_key_press(void) {


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


