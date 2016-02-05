// Luke Soldano, Xavier Agostini (C) 2016
// McGill University, ECSE 426, Lab One
// Assembly code to test and access various Kalmann filters

#include <stdio.h>
#include <math.h>
#include "arm_math.h"
#include "demo.h"

#define ARRAY_LENGTH length
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
	

int call_asm(float* inputArray, float* outputArray, int Length, kalman_t* kstate);
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length);
int dsp_c(float* inputArray, float* outputArray, dsp_t* analysisOut);
int dsp_arm(float* inputArray, float* outputArray, dsp_t* analysisOut);

// Main function *******************************************************************************************
int main() {
	
	kalman_t kalmanState_asm, kalmanState_c;
	dsp_t out_c, out_arm;
	
	int i;
	
	float measurements[] =
{
-0.665365,
-0.329988,
0.164465,
0.043962,
0.295885};
//-0.643138,
//0.615203,
//-0.254512,
//0.261842,
//0.014163,
//0.045181,
//0.554502,
//0.198915,
//0.120703,
//-0.547104,
//0.103219,
//-0.204776,
//0.107782,
//-0.105263,
//0.356157};
//0.172390,
//-0.154121,
//0.134996,
//0.392204,
//0.204622,
//0.006685,
//0.493516,
//0.462641,
//0.103078,
//0.309435,
//0.213690,
//0.092270,
//0.135294,
//-0.227323,
//0.546517,
//1.342128,
//0.548172,
//0.687299,
//0.932445,
//0.115749,
//0.306152,
//-0.110230,
//0.761805,
//0.345018,
//0.529871,
//-0.127723,
//1.029281,
//0.831176,
//0.442460,
//1.515545,
//0.188082,
//0.218138,
//0.885141,
//0.940009,
//0.495364,
//0.577697,
//0.553857,
//0.522537,
//0.607049,
//0.659359,
//0.963857,
//1.054525,
//0.843292,
//0.810490,
//0.570706,
//0.357432,
//0.758009,
//0.536896,
//0.387944,
//1.257416,
//0.993856,
//1.457375,
//1.261647,
//0.659197,
//0.485025,
//0.559893,
//0.695033,
//0.450578,
//0.977574,
//0.619640,
//0.549591,
//1.091964,
//1.141594,
//1.063596,
//1.339442,
//1.154712,
//1.113118,
//0.699106,
//0.675865,
//0.950326,
//1.278943,
//1.152681,
//1.261983,
//1.131821,
//1.522107,
//0.714389,
//2.134284,
//1.210318,
//0.908917,
//1.036300
//};
	
	//float testArray[ARRAY_LENGTH] = {0.1, 0.1, 0.2, 0.3, 0.4};
	float outputArray_c[ARRAY_LENGTH];
	float outputArray_asm[ARRAY_LENGTH];
	float *inputArray = measurements;
	
	// PART II Call ASM function 
	if (call_asm(inputArray, outputArray_asm, ARRAY_LENGTH, &kalmanState_asm) != 0) { 
		printf("Error in ASM function");
		return -1;
	}
		
	// PART II Outputs - ASM code implementation
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_asm[%i] = %0.6f\n", i, outputArray_asm[i]);
	
	
	// PART II Call C function to update kalman state and print output
	if (Kalmannfilter_C(inputArray, outputArray_c, &kalmanState_c, ARRAY_LENGTH) != 0) { 
		printf("Error in C function");
		return -1;
	}
	
	// PART II Outputs - C code implementation
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_c[%i] = %0.6f\n", i, outputArray_c[i]);
	
	
	// PART III Outputs - Our own implementation
	printf("Part 3: Our C DSP implementation\n");
	dsp_c(inputArray, outputArray_c, &out_c);
	
	for (i=0; i < ARRAY_LENGTH; i++) printf("diffArray_c[%i] = %0.9f\n", i, out_c.diffArr[i]);
	printf("mean: %.6f\n", out_c.meanDiff);
	printf("standard deviation: %.9f\n", out_c.standDevDiff);
	for (i=0; i < ARRAY_LENGTH; i++) printf("correlationArr[%i] = %0.9f\n", i, out_c.corrArr[i]);
	for (i=0; i < ARRAY_LENGTH; i++) printf("convolutionArr[%i] = %0.9f\n", i, out_c.convolArr[i]);

	
	// PART III Outputs - CMSIS-DSP implementation (TODO : MOVE PRINT STATEMENTS OUTSIDE OF THIS FUNCTION)
	printf("Part 3: CMSIS-DSP implementation\n");
	dsp_arm(inputArray, outputArray_c, &out_arm);
	
	for (i=0; i < ARRAY_LENGTH; i++) printf("diffArr[%i] = %0.9f\n", i, out_arm.diffArr[i]);
	printf("diffMean: %.9f\n", out_arm.meanDiff);
	printf("standardDev: %.9f\n", out_arm.standDevDiff);
	for (i=0; i < ARRAY_LENGTH; i++) printf("corrArr[%i] = %0.9f\n", i, out_arm.corrArr[i]);
	for (i=0; i < ARRAY_LENGTH; i++) printf("convolArr[%i] = %0.9f\n", i, out_arm.convolArr[i]);
	
	
	return 0;
}

// Call to ASM Kalman filter function ***********************************************************************
int call_asm(float* inputArray, float* outputArray, int Length, kalman_t* kstate){
	
	// Set initial values of kstate
	kstate->q = DEF_q;
	kstate->r = DEF_r;
	kstate->x = DEF_x; 
	kstate->p = DEF_p;
	kstate->k = DEF_k;
	
	// Call assembly function to update kalman state with input
	return Kalmanfilter_asm(inputArray, outputArray, length, kstate);
	
}

// C Kalman filter function *********************************************************************************
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length){
	
	int i;
	
	// Set initial values of kstate
	kstate->q = DEF_q;
	kstate->r = DEF_r;
	kstate->x = DEF_x;
	kstate->p = DEF_p;
	kstate->k = DEF_k;
	
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
	kstate->x = kstate->x + kstate->k * (InputArray[0] - kstate->x);
	kstate->p = (1 - kstate->k) * kstate->p;
	OutputArray[0] = kstate->x;
	
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
int dsp_c(float* inputArray, float* outputArray, dsp_t* analysisOut) {
	
	int i, k, tempLength;
	
	float32_t meanDiff, stdDevDiff, tempSum;
	float32_t diffArray[ARRAY_LENGTH], correlationArray[2*ARRAY_LENGTH-1];
	
	
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
	
	tempLength = 2*ARRAY_LENGTH - 1;
	
	// Calculate correlation array
	for (i=0; i < tempLength; i++){
		for (k=0; k < tempLength; k++){
			if (i < tempLength/2){
				if (k <= i){
					correlationArray[i] += inputArray[k] * outputArray[tempLength/2 + k - i];
				}
			}
			else{
				if (k <= tempLength/2 && tempLength/2 + k - i >= 0){
					correlationArray[i] += inputArray[k] * outputArray[tempLength/2 + k - i];
				}
			}
		}
		
		analysisOut->corrArr[i] = correlationArray[i];
	}

	// Calculate convolution between the 2 vectors
	//// y[n] = x[n]*h[n] = sum_{-5,5} x[k] * h[n-k]
	for(i=0; i<ARRAY_LENGTH; i++) {
		for(k=0; k<ARRAY_LENGTH;k++) {
			if(i-k >= 0) tempSum += inputArray[k] * outputArray[i - k];
		}

		analysisOut->convolArr[i] = tempSum;
		tempSum = 0.0;
	}
	
	analysisOut->meanDiff = meanDiff;
	analysisOut->standDevDiff = stdDevDiff;
	
	
	return 0;
}

// ARM DSP Function ****************************************************************************************
int dsp_arm(float* inputArray, float* outputArray, dsp_t* analysisOut) {
	
	// Get diffence Array
	arm_sub_f32(outputArray, inputArray, analysisOut->diffArr, ARRAY_LENGTH);
	
	// Get mean
	arm_mean_f32(analysisOut->diffArr, (uint32_t) ARRAY_LENGTH, &analysisOut->meanDiff);
	
	// Get standard deviation
	arm_std_f32(analysisOut->diffArr, (uint32_t) ARRAY_LENGTH, &analysisOut->standDevDiff);
	
	// Get correlation
	arm_correlate_f32(inputArray, ARRAY_LENGTH, outputArray, ARRAY_LENGTH, analysisOut->corrArr);
	
	// Get convolution
	arm_conv_f32(inputArray, ARRAY_LENGTH, outputArray, ARRAY_LENGTH, analysisOut->convolArr);
	
	
	return 0;
}
