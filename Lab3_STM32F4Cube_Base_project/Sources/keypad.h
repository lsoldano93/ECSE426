#ifndef KEYPAD_H
#define KEYPAD_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#define ROWS 4
#define COLS 3

void init_keypad(void);
uint8_t get_column(void);
uint8_t get_row(void);
int get_key(void);

// 11 indicates '*' and 12 indicates '#'
const uint8_t keypad_map[ROWS][COLS] = {
	{1, 2, 3},
	{4, 5, 6},
	{7, 8, 9},
	{11, 0, 12}};

#endif