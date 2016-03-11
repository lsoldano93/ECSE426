

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "keypad.h"

/* Private variables ---------------------------------------------------------*/

// Pin maps for keypad columns and rows
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
	
void init_rows(void) {
	
	/* 7-Segment Display Pinout {1:CCD1, 2:CCD2, 3:D, 4:Degree, 5:E, 6:CCD3, 
	   7:DP, 8:CCD4, 9:, 10:, 11:F, 12:, 13:C, 14:A, 15:G, 16:B}
	
		 Keypad Pinout {1:R1, 2:R2, 4:R4, 5:R5, 6:C1, 7:C2, 8:C3}
	
	   GPIO Pin Mapping for Display
		 Segment Ctrls {CCD1:PC1, CCD2:PC2, CCD3:PC4, CCD4:PC5}
	   DP & DC Ctrl  {DP:PC6, DC:PC7}
		 Segments      {A:PB0, B:PB1, C:PB5, D:PB11, E:PB12, F:PB13, G:PB14} 
	
		 GPIO Pin Mapping for Keypad
		 Rows: {R1:PD1, R2:PD2, R3:PD6 , R4:PD7}
		 Columns: {C1:PD8, C2:PD9, C3:PD10}
	
		 Alarm LED : PD13
	*/
		
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	//initialize rows
	GPIO_row.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7; 
	
	GPIO_row.Mode = GPIO_MODE_IT_RISING;
	GPIO_row.Pull = GPIO_PULLUP;
	GPIO_row.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_row);
	
	// Initialize columns
	GPIO_col.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	
	GPIO_col.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_col.Pull = GPIO_NOPULL;
	GPIO_col.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_col);
	
}

void init_columns(void) {
	
	/*
		 Keypad Pinout {1:R1, 2:R2, 4:R4, 5:R5, 6:C1, 7:C2, 8:C3}
		 GPIO Pin Mapping
		 Rows: {R1:D1, R2:D2, R3:D6 , R4:D7}
		 Columns: {C1:D8, C2:D9, C3:D10}
		 Alarm LED : PD13
	*/
	
	//Reset bit on rows
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_6);
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_7);

	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	//initialize rows
	GPIO_row.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_6 | GPIO_PIN_7; 
	
	GPIO_row.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_row.Pull = GPIO_NOPULL;
	GPIO_row.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_row);
	
	// Initialize columns
	GPIO_col.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 ;
	
	GPIO_col.Mode = GPIO_MODE_IT_RISING;
	GPIO_col.Pull = GPIO_PULLUP;
	GPIO_col.Speed =  GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_col);
	
}

/**  Uses timer 3 to generate delay for display
   * @brief  Allows for software delay to be used for display purposes **/
void delay(uint32_t time)
{
	
  TimmingDelay = time;
  while(TimmingDelay !=0);
	
}  


/**
   * @brief Reports column of key that was pressed
   * @retval Int of column activated
   */
uint8_t get_column(void) {
	
	// Initialize pins to allow for columns as input
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
	
	// Initialize pins to allow for rows as input
	init_rows();
	
	// Row selection determined by active low (TODO find pins)
	if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[3])) return 3;
	else if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[2])) return 2;
	else if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[1])) return 0;
	else if(!HAL_GPIO_ReadPin(GPIOD, row_pinmap[0])) return 1;
	
	else return 9;

}


/**
   * @brief Determines row and column of key press then returns value from map
   * @retval Returns exact key pressed
   */
int get_key(void) {
	
	int i = 0;
	
	uint8_t key;
	uint8_t row, column;
	
	// Interupt handling, and debouncing;
	if((column = get_column()) == 9) return -1;
	if((row = get_row()) == 9) return -1;
	
	// Hold code in wait state until button released/held long enough
	key = keypad_map[row][column];
	while(1){
		
		// Wait state
		while(keypad_map[get_row()][get_column()] == key){ 
			i++;
			delay(25); 
			//printf("%d\n",key);
		}
		
		// Key held long enough and isn't noise
		if(i > 2) break;
		else return get_key();
		
	}

	printf("%d made it dad!\n", key);
	return key;		
	
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
	if((key = get_key()) == -1 || key == 0 || key == 11 || key == 12) return -1;
	
	angle = key;
	
	while(1){
		
		// Wait for key press to be relavant
		while((key = get_key()) == -1 || key == 11);
		
		if (key != 12) angle = angle * 10 + key;
		else return angle;
		
		if((i += 1) == 2) break;
	}
	
	while((key = get_key()) != 12);
	
	return angle;
	
}


