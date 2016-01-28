#include <stdio.h>
#include "arm_math.h"

#define ARRAY_LENGTH 5

typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

void Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);

int main() {
	
	kalman_t *kalman;

	float inputArray[ARRAY_LENGTH] = {0.1, 0.1, 0.0, 0.0, 0.0};
	float outputArray[ARRAY_LENGTH] = {0.0, 0.0, 0.0, 0.0, 0.0};

	float *input = inputArray;
	float *output = outputArray;
	
	// Call assembly function to update kalman state with input
	Kalmanfilter_asm(input, output, ARRAY_LENGTH, kalman);
	
	return 0;
}
