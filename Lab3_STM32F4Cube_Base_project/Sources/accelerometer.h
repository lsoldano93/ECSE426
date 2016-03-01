#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>
#include "lis3dsh.h"
#include "math.h"

void init_interrupts(void);
void init_accelerometer(void); 

void update_accel_values(float Ax, float Ay, float Az);
float calc_roll_angle(void);
float calc_pitch_angle(void);
float calc_yaw_angle(void);
#endif