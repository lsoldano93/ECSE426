#include <stdio.h>
#include "keypad.h"
//
const uint16_t col_pinmap[3] = {GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10};
const uint16_t row_pinmap[4] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_6, GPIO_PIN_7};

// 11 indicates '*' and 12 indicates '#'
const uint8_t keypad_map[4][3] = {
	{1, 2, 3},
	{4, 5, 6},
	{7, 8, 9},
	{11, 0, 12}};

/* Private functions ---------------------------------------------------------*/

	GPIO_InitTypeDef GPIO_row, GPIO_col;
/**
   * @brief Initializes keypad for proper operation by configuring GPIO pins as input
   */

/*
		Keypad (8 pins needed)
	Rows: D1, D2, D6 , D7
	Columns: D8, D9, D10

*/
void init_rows(void) {
	
	//Reset bits on columns
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_8);
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
		
	//initialize GPIO rows of the keypad to active low

	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	//initialize rows
	GPIO_row.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7; 
	
	GPIO_row.Mode = GPIO_MODE_IT_RISING;
	GPIO_row.Pull = GPIO_PULLUP;
	GPIO_row.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_row);
	
	
	//__HAL_RCC_GPIOD_CLK_ENABLE();
	
	// initialize colums
	GPIO_col.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	
	GPIO_col.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_col.Pull = GPIO_NOPULL;
	GPIO_col.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_col);
}

void init_columns(void) {
	
	//Reset bit on rows
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
//	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);
	
		//initialize GPIO rows of the keypad to active low

	//__HAL_RCC_GPIOD_CLK_ENABLE();
	
	//initialize rows
	GPIO_row.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7; 
	
	
	GPIO_row.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_row.Pull = GPIO_NOPULL;
	GPIO_row.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_row);
	
	
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	// initialize colums
	GPIO_col.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	
	GPIO_col.Mode = GPIO_MODE_IT_RISING;
	GPIO_col.Pull = GPIO_PULLUP;
	GPIO_col.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_col);
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
	init_columns();
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
	init_rows();
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
	

	
	//if(!debounce(col_pinmap[column], row_pinmap[row])) return -1;
	
	//return keypad_map[row][column];		
	return row;
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


