#ifndef 7_SEGMENT_H
#define 7_SEGMENT_H

#include "stm32f4xx_hal_gpio.h"

void display_init(void);
void selectDigit(int digit);
void lightNum(int num);
void draw_arrow(int sign);
void draw_decimal(int digit);
void draw_degree(void);

#endif