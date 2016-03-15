#ifndef THREAD_USERINTERFACE_H
#define THREAD_USERINTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "global_vars.h"
#include "stm32f4xx_hal_gpio.h"

#define UI_THREAD_OSDELAY 5  // Delay in milliseconds
#define DEBOUNCE_DELAY 25    // Delay in milliseconds

#define ROWS 4
#define COLS 3

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**  Initiates user interface thread
   * @brief  Builds thread and starts it
   * @retval Integer inidicating failure or success of thread initiation
   */
int start_Thread_UserInterface (void);

/**  User interface thread 
   * @brief  Runs UI thread which reads key presses and updates the display accordingly
   */
void Thread_UserInterface (void const *argument)

/**  Handle key presses
   * @brief Pressing 0 toggles temperature/accelerometer; pressing 1/2 toggles tilt angles
   */
void handleKeyPress(void);

/**  Causes the display of a temperature value
   * @brief  Draws temperature on display
	 * @param  Float of temperature to be displayed **/
void drawTemperature(float temp);

/**  Causes the display of an angle value
   * @brief  Draws angle on display
	 * @param  Float of angle to be displayed **/
void drawAngle(float angle);

/**
   * @brief Determines row and column of key press then returns value from map
   * @retval Returns exact key pressed **/
int getKey(void);

/**
   * @brief Reports column of key that was pressed
   * @retval Int of column activated **/
uint8_t getColumn(void);

/**
   * @brief Reports row of key that was pressed
   * @retval Int of row activated **/
uint8_t getRow(void);

/**  Initialization of pins to allow for row read
   * @brief  Sets row pins as input pullup and column pins as output no pull **/
void init_rows(void);

/**  Initialization of pins to allow for column read
   * @brief  Sets column pins as input pullup and row pins as output no pull **/
void init_columns(void);

/**  Selects segment digit to display for
   * @brief  Cycles through cases for each segment of display and enables/disables proper pins
	 * @param  Segment digit to be illuminated **/
void selectDigit(int digit);

/**  Lights segment digit selected with proper display
   * @brief  Takes in value to be displayed on preselected 7 segment display
	 * @param  Value to be displayed **/
void lightNum(int num);

/**  Resets all display pins
   * @brief  "" **/
void resetPins();

/**  Configures system for user interface
   * @brief  Initializes pins for display and clock for keypad **/
void UserInterface_config(void);

/**  Uses timer 3 to generate delay for display
   * @brief  Allows for software delay to be used for display purposes **/
void delay(uint32_t time);

#endif