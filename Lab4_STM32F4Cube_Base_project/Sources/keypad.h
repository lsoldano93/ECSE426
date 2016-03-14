#ifndef KEYPAD_H
#define KEYPAD_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "global_vars.h"

#define ROWS 4
#define COLS 3
#define DEBOUNCE_DELAY 25 // 50 ms

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**  Initialization of pins to allow for row read
   * @brief  Sets row pins as input pullup and column pins as output no pull **/
void init_rows(void);

/**  Initialization of pins to allow for column read
   * @brief  Sets column pins as input pullup and row pins as output no pull **/
void init_columns(void);

/**  Uses timer 3 to generate delay for display
   * @brief  Allows for software delay to be used for display purposes **/
void delay(uint32_t time);

/**
   * @brief Reports column of key that was pressed
   * @retval Int of column activated **/
uint8_t get_column(void);

/**
   * @brief Reports row of key that was pressed
   * @retval Int of row activated **/
uint8_t get_row(void);

/**
   * @brief Determines row and column of key press then returns value from map
   * @retval Returns exact key pressed **/
int get_key(void);

/**  Handle key presses
   * @brief Pressing 0 toggles temperature/accelerometer; pressing 1/2 toggles tilt angles
   */
void handle_key_press(void);



#endif