#include <stdio.h>
#include "arm_math.h"
//#include "test.h"

#define ARRAY_LENGTH 5

typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

void Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length);

int main() {
	
	kalman_t *kalmanState_asm, *kalmanState_c;
	
	float *input, *output;
	float inputArray[ARRAY_LENGTH] = {0.0, 1.1, 2.1, 3.0, 2.0};
	float outputArray[ARRAY_LENGTH] = {0.0, 0.0, 0.0, 0.0, 0.0};
	
	kalmanState_asm->q = 0.1;
	kalmanState_c->q = 0.1;
	
	kalmanState_asm->r = 0.1;
	kalmanState_c->r = 0.1;
	
	kalmanState_asm->x = 0.0;
	kalmanState_c->x = 0.0;
	
	kalmanState_asm->p = 0.0;
	kalmanState_c->p = 0.0;
	
	kalmanState_asm->k = 0.0;
	kalmanState_c->k = 0.0;

	input = inputArray;
	output = outputArray;
	
	// Call assembly function to update kalman state with input
	Kalmanfilter_asm(input, output, ARRAY_LENGTH, kalmanState_asm);
	
	return 0;
}

int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length){
	
	kstate->p = kstate->p + kstate->q;
	kstate->k = kstate->p / (kstate->p + kstate->r);
	kstate->x = kstate->x + kstate->k * (InputArray[0] - kstate->x);
	kstate->p = (1 - kstate->k) * kstate->p;
	
	// TODO: ask about output array and purpose
	//OutputArray[
	
	return (int) kstate->x;
}
