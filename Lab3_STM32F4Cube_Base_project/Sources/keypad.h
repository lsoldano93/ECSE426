#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "global_vars.h"

#define ROWS 4
#define COLS 3
#define DEBOUNCE_DELAY 50 // ms

/* Private function prototypes -----------------------------------------------*/

/**
   * @brief Initializes keypad for proper operation by configuring GPIO pins as input
   */
void init_keypad(void);

/**
   * @brief Reports whether or not key is still active low after DEBOUNCE_DELAY ms
	 * @param Row and col GPIO pins associated with D-set where debounce should be checked
   * @retval 1 if proper value read, 0 if effects due to bounce detected
   */
uint8_t debounce(uint16_t COL_PIN, uint16_t ROW_PIN);

/**
   * @brief Reports column of key that was pressed
   * @retval Int of column activated
   */
uint8_t get_column(void);

/**
   * @brief Reports row of key that was pressed
   * @retval Int of row activated
   */
uint8_t get_row(void);

/**
   * @brief Determines row and column of key press then returns value from map
   * @retval Returns exact key pressed
   */
int get_key(void);

/**
   * @brief Reports action on keypad
   * @retval -1 if no key pressed yet, angle value if they have
   */
int handle_key_press(void);

/* Private variables ---------------------------------------------------------*/

const uint16_t col_pinmap[COLS] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_6};
const uint16_t row_pinmap[ROWS] = {GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10};

// 11 indicates '*' and 12 indicates '#'
const uint8_t keypad_map[ROWS][COLS] = {
	{1, 2, 3},
	{4, 5, 6},
	{7, 8, 9},
	{11, 0, 12}};

#endif