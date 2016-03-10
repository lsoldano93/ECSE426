#ifndef DISPLAY_SEGMENT_H
#define DISPLAY_SEGMENT_H

#include "stm32f4xx_hal_gpio.h"
#include "global_vars.h"

void init_display(void);
void selectDigit(int digit);
void lightNum(int num);
void draw_angle(float angle);
void draw_d(void);
void draw_u(void);
void reset(void);
void Delay(uint32_t time);


#endif