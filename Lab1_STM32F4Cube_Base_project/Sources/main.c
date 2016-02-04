#include <stdio.h>
#include <math.h>
#include "arm_math.h"
//#include "test.h"

#define ARRAY_LENGTH 5
#define MAX_VALUE 3.4 * pow(10,38)

// Struct declarations *************************************************************************************
typedef struct kalman_t{
	float q;
	float r;
	float x;
	float p;
	float k;
} kalman_t;

typedef struct dsp_t {
	float32_t diffArr[ARRAY_LENGTH];
	float32_t meanDiff;
	float32_t standDevDiff;
	float32_t corrArr[ARRAY_LENGTH];
	float32_t convolArr[ARRAY_LENGTH];
	
} dsp_t;
	

int call_asm(float* testArray, float* outputArray, int length, kalman_t* kstate);
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length);
int func(float* inputArray, float* outputArray, dsp_t* analysisOut);
int funcArm(float* inputArray, float* outputArray, dsp_t* analysisOut);

// Main function *******************************************************************************************
int main() {
	
	kalman_t kalmanState_asm, kalmanState_c;
	dsp_t out_c, out_arm;
	
	int i;
	
	float testArray[ARRAY_LENGTH] = {0.1, 0.1, 0.2, 0.3, 0.4};
	float outputArray_c[ARRAY_LENGTH];
	float outputArray_asm[ARRAY_LENGTH];
	
	
	// PART II Call ASM function 
//	if (call_asm(testArray, outputArray_asm, ARRAY_LENGTH, &kalmanState_asm) != 0) { 
//		printf("Error in ASM function");
//		return -1;
//	}
//		
	// PART II Outputs - ASM code implementation
//	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_asm[%i] = %f\n", i, outputArray_asm[i]);
//	
	
	// PART II Call C function to update kalman state and print output
	if (Kalmannfilter_C(testArray, outputArray_c, &kalmanState_c, ARRAY_LENGTH) != 0) { 
		printf("Error in C function");
		return -1;
	}
	
	// PART II Outputs - C code implementation
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_c[%i] = %f\n", i, outputArray_c[i]);
	
	
	// PART III Outputs - Our own implementation
	printf("Part 3: Our personal C implementation\n");
	func(testArray, outputArray_c, &out_c);
	for (i=0; i < ARRAY_LENGTH; i++) printf("diffArray_c[%i] = %f\n", i, out_c.diffArr[i]);
	printf("mean: %.9f\n", out_c.meanDiff);
	printf("standard deviation: %.9f\n", out_c.standDevDiff);
	for (i=0; i < ARRAY_LENGTH; i++) printf("correlationArr[%i] = %f\n", i, out_c.corrArr[i]);
	for (i=0; i < ARRAY_LENGTH; i++) printf("convolutionArr[%i] = %f\n", i, out_c.convolArr[i]);

	
	// PART III Outputs - CMSIS-DSP implementation (TODO : MOVE PRINT STATEMENTS OUTSIDE OF THIS FUNCTION)
	printf("Part 3: CMSIS-DSP implementation\n");
	funcArm(testArray, outputArray_c, &out_arm);
	for (i=0; i < ARRAY_LENGTH; i++) printf("diffArr[%i] = %0.9f\n", i, out_arm.diffArr[i]);
	printf("diffMean: %.9f\n", out_arm.meanDiff);
	printf("standardDev: %.9f\n", out_arm.standDevDiff);
	for (i=0; i < ARRAY_LENGTH; i++) printf("corrArr[%i] = %0.9f\n", i, out_arm.corrArr[i]);
	for (i=0; i < ARRAY_LENGTH; i++) printf("convolArr[%i] = %0.9f\n", i, out_arm.convolArr[i]);
	
	
	return 0;
}

// Call to ASM Kalman filter function ***********************************************************************
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

// C Kalman filter function *********************************************************************************
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length){
	
	int i;
	
	// Set initial values of kstate
	kstate->q = 0.1;
	kstate->r = 0.1;
	kstate->x = 0.0;
	kstate->p = 0.0;
	kstate->k = 0.0;
	
	// Run unique iteration on first run where overflow can occur
	//// Check for overflow in input value or addition
	if ( (kstate->p = kstate->p + kstate->q) >= MAX_VALUE || InputArray[0] >= MAX_VALUE){
		return -1;
	}
	
	// Check for overflow and NAN
	if ( (kstate->p + kstate->r) == 0 || (kstate->p + kstate->r) >= MAX_VALUE){
			return -1;
	}
	
	kstate->k = kstate->p / (kstate->p + kstate->r);
	kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
	kstate->p = (1 - kstate->k) * kstate->p;
	OutputArray[i] = kstate->x;
	
	for (i=1; i < Length; i++){
		
		// Check for input measurement overflow
		if ( InputArray[i] >= MAX_VALUE ){
			return -1;
		}
		
		kstate->p = kstate->p + kstate->q;
		
		// Check for NAN
		if ( (kstate->p + kstate->r) == 0 ){
			return -1;
		}
		
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[i] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;
		OutputArray[i] = kstate->x;
		
	}
	
	return 0;
}

// C DSP Function *******************************************************************************************
int func(float* inputArray, float* outputArray, dsp_t* analysisOut) {
	
	int i,k;
	
	float32_t meanDiff, stdDevDiff, tempSum;
	float32_t diffArray[ARRAY_LENGTH], correlationArray[ARRAY_LENGTH], convolArray[ARRAY_LENGTH];
	
	
	meanDiff = stdDevDiff = tempSum = 0.0;
	
	// Subtraction of original data and filtered data
	for (i=0; i < ARRAY_LENGTH; i++) {
		analysisOut->diffArr[i] = diffArray[i] = outputArray[i] - inputArray[i];
		meanDiff += diffArray[i];
	}
	
	// Standard deviation: sigma = sqrt((1/N) sum(x_i-u)^2)	; also calculate average/mean of difference array
	meanDiff = meanDiff/ARRAY_LENGTH;	
	
	for (i=0; i < ARRAY_LENGTH; i++) {
		stdDevDiff += pow((diffArray[i] - meanDiff),2); 
	}
	
	stdDevDiff = sqrt((1.0/ARRAY_LENGTH)*stdDevDiff);
	
	// Calculate correlation array
	for (i=0; i < ARRAY_LENGTH; i++){
		for (k=0; k < ARRAY_LENGTH; k++){
			correlationArray[i] += (inputArray[k] * outputArray[ARRAY_LENGTH - (k + i)]);
		}
	}

	// Calculate convolution between the 2 vectors
	//// y[n] = x[n]*h[n] = sum_{-5,5} x[k] * h[n-k]
	for(i=0; i<ARRAY_LENGTH; i++) {
		for(k=0; k<ARRAY_LENGTH;k++) {
			if(i-k >= 0) tempSum += inputArray[k] * outputArray[i - k];
		}

		analysisOut->convolArr[i] = convolArray[i] = tempSum;
		analysisOut->corrArr[i] = correlationArray[i];
		tempSum = 0.0;
	}
	
	analysisOut->meanDiff = meanDiff;
	analysisOut->standDevDiff = stdDevDiff;
	
	
	return 0;
}

// ARM DSP Function ****************************************************************************************
int funcArm(float* inputArray, float* outputArray, dsp_t* analysisOut) {
	
	int i;
	
	float32_t diffArr[ARRAY_LENGTH];
	float32_t diffMean, stanDev;
	float32_t corrArr[ARRAY_LENGTH];
	float32_t convolArr[ARRAY_LENGTH];
	
	// Get diffence Array
	arm_sub_f32(outputArray, inputArray, analysisOut->diffArr, 5.0);
	
	// Get mean
	arm_mean_f32(analysisOut->diffArr, (uint32_t) 5, &analysisOut->meanDiff);
	
	// Get standard deviation
	arm_std_f32(analysisOut->diffArr, (uint32_t) 5, &analysisOut->standDevDiff);
	
	// Get correlation
	arm_correlate_f32(inputArray, 5.0, outputArray, 5.0, analysisOut->corrArr);
	
	// Get convolution
	arm_conv_f32(inputArray, 5, outputArray, 5, analysisOut->convolArr);
	
	
	return 0;
}
