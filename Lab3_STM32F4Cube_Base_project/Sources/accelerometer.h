#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "lis3dsh.h"
#include "math.h"

#define PI 3.14159265358

//need to tun ACC10, ACC20, ACC30
#define ACC11 1 
#define ACC12 0 
#define ACC13 0
#define ACC10 0 //Add to x
#define ACC21 0
#define ACC22 1
#define ACC23 0
#define ACC20 0 //Add to y
#define ACC31 0
#define ACC32 0
#define ACC33 1
#define ACC30 70 //Add to the z 

#define DEGREES(x) (180.0*x/PI)

typedef struct {
	float x; 
	float y; 
	float z;
} accelerometer_values;


void init_interrupts(void);
void init_accelerometer(void); 

void update_accel_values(float Ax, float Ay, float Az);
float calc_roll_angle(void);
float calc_pitch_angle(void);
float calc_yaw_angle(void);
#endif