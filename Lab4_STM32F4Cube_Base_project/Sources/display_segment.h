#ifndef DISPLAY_SEGMENT_H
#define DISPLAY_SEGMENT_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_gpio.h"
#include "global_vars.h"
#include "cmsis_os.h"

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/ 

/**  Initializes GPIO pins for 7 segment display
   * @brief  Initializes pins for display **/
void init_display(void);

/**  Selects segment digit to display for
   * @brief  Cycles through cases for each segment of display and enables/disables proper pins
	 * @param  Segment digit to be illuminated **/
void selectDigit(int digit);

/**  Lights segment digit selected with proper display
   * @brief  Takes in value to be displayed on preselected 7 segment display
	 * @param  Value to be displayed **/
void lightNum(int num);

/**  Causes the display of a temperature value
   * @brief  Draws temperature on display
	 * @param  Float of temperature to be displayed **/
void draw_temperature(float temp);

/**  Causes the display of an angle value
   * @brief  Draws angle on display
	 * @param  Float of angle to be displayed **/
void draw_angle(float angle);

/**  Resets all display pins
   * @brief  "" **/
void reset();


#endif