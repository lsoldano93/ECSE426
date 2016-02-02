#include <stdio.h>
#include <math.h>
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

int call_asm(float* testArray, float* outputArray, int length, kalman_t* kstate);
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length);

int main() {
	
	kalman_t kalmanState_asm, kalmanState_c;
	
	int i, returnCode;
	
	float testArray[ARRAY_LENGTH] = {0.1, 0.1, 0.2, 0.3, 0.4};
	float outputArray_c[ARRAY_LENGTH];
	float outputArray_asm[ARRAY_LENGTH];
	
	// Part 3 variables
	float differenceArray_c[ARRAY_LENGTH], differenceArray_asm[ARRAY_LENGTH];
	float meanDiff_c = 0.0, meanDiff_asm =  0.0;
	float stanDev_c =  0.0, stanDev_asm =  0.0;
	float avg_c = 0.0, avg_asm =  0.0; 
	float correlation_c = 0.0, correlation_asm = 0.0;
	
	// Call assembly function and print output array
	returnCode = call_asm(testArray, outputArray_asm, ARRAY_LENGTH, &kalmanState_asm);
	if (returnCode != 0) { 
		printf("Error in assembly function");
		return -1;
	}
		
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_asm[%i] = %f\n", i, outputArray_asm[i]);
	
	// Call C function to update kalman state and print output
	returnCode = Kalmannfilter_C(testArray, outputArray_c, &kalmanState_c, ARRAY_LENGTH);
	if (returnCode != 0) { 
		printf("Error in C function");
		return -1;
	}
	
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_c[%i] = %f\n", i, outputArray_c[i]);
	
	// TODO: subtract output arrays to analyze difference in values
	// PART III: 4 Additional Operations
	
	//A: Subtraction of original data and filtered data
	
	for (i=0; i < ARRAY_LENGTH; i++) {
		differenceArray_c[i] = outputArray_c[i] - testArray[i];
		meanDiff_c += differenceArray_c[i];
		
		differenceArray_asm[i] = outputArray_asm[i] - testArray[i];
		meanDiff_asm += differenceArray_asm[i];
	}
	meanDiff_c = meanDiff_c/ARRAY_LENGTH;
	meanDiff_asm = meanDiff_asm/ARRAY_LENGTH;
	
	//B: Standard deviation: sigma = sqrt((1/N) sum(x_i-u)^2)	; also calculate average of difference array
	
	for (i=0; i < ARRAY_LENGTH; i++) {
		stanDev_c += pow((differenceArray_c[i] - meanDiff_c),2); 
		avg_c += differenceArray_c[i];
		
		stanDev_asm += pow((differenceArray_asm[i] - meanDiff_asm),2);
		avg_asm += differenceArray_asm[i];
	}
	stanDev_c = sqrt((1/ARRAY_LENGTH)*stanDev_c);
	avg_c = avg_c / ARRAY_LENGTH;
	
	stanDev_asm = sqrt((1/ARRAY_LENGTH)*stanDev_asm);
	avg_asm = avg_asm / ARRAY_LENGTH;
	
	//C: Calculate correlation coefficient between original and tracked vectors
	for (i=0; i < ARRAY_LENGTH; i++) {
//		xy += testArray[i] * outputArray_c[i] ;
//		x += testArray[i];
//		y += outputArray_c[i]
//		x^2 += pow(testArray[i],2);
//		y^2 += pow(outputArray_c[i],2);
	}
	//D: Calculate convolution between the 2 vectors
	
	return 0;
}

int call_asm(float* testArray, float* outputArray, int length, kalman_t* kstate){
	
	// Set initial values of kstate
	kstate->q = 0.1;
	kstate->r = 0.1;
	kstate->x = 0.0;
	kstate->p = 0.0;
	kstate->k = 0.0;
	
	// Call assembly function to update kalman state with input
	return Kalmanfilter_asm(testArray, outputArray, length, kstate);
	
}

int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length){
	
	int i;
	
	// Set initial values of kstate
	kstate->q = 0.1;
	kstate->r = 0.1;
	kstate->x = 0.0;
	kstate->p = 0.0;
	kstate->k = 0.0;
	
	for (i=0; i < Length; i++){
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;
		OutputArray[i] = kstate->x;
	}
	
	return 0;
}
