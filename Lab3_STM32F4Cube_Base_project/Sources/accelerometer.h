#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "lis3dsh.h"
#include "math.h"

/* Private typedef -----------------------------------------------------------*/

typedef struct {
	float x; 
	float y; 
	float z;
} accelerometer_values;

#define PI 3.14159265358
#define ACC11 0.9523
#define ACC12 -0.0019
#define ACC13 -0.0079
#define ACC10 -0.0297
#define ACC21 -0.0397
#define ACC22 0.9896
#define ACC23 -0.0105
#define ACC20 0.0070
#define ACC31 -0.0047
#define ACC32 -0.0024
#define ACC33 1.0087
#define ACC30 -0.0328 
#define DEGREES(x) (180.0*x/PI)

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/ 

/**  Accelerometer bread and butter
   * @brief  Updates x, y, z parameters of accelerometer by reading from MEMs device
	 * @param  Locations where updated values will be stored **/
void update_accel_values(float Ax, float Ay, float Az);

/**  Calculates pitch angle
   * @brief  Calculates pitch angle
   * @retval Returns pitch angle **/
float calc_pitch_angle(void);

/**  Calculates roll angle
   * @brief  Calculates roll angle
   * @retval Returns roll angle **/
float calc_roll_angle(void);

/**  Calculates yaw angle
   * @brief  Calculates yaw angle
   * @retval Returns yaw angle **/
float calc_yaw_angle(void);

/**  Initialize accelerometer
   * @brief  Initializes accelerometer for use **/
void init_accelerometer(void);



#endif