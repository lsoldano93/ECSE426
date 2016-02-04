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

typedef struct dsp_t {
	float diffArr[ARRAY_LENGTH];
	float meanDiff;
	float standDevDiff;
	float correlation;
	float convolArr[ARRAY_LENGTH];
	
} dsp_t;
	

int call_asm(float* testArray, float* outputArray, int length, kalman_t* kstate);
int Kalmanfilter_asm(float* inputArray, float* outputArray, int arrayLength, kalman_t* kalman);
int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length);
struct dsp_t func(float* inputArray, float* outputArray);
struct dsp_t funcArm(float* inputArray, float* outputArray);
int main() {
	
	kalman_t kalmanState_asm, kalmanState_c;
	dsp_t out_c, out_arm;
	
	int i, returnCode;
	
	float testArray[ARRAY_LENGTH] = {0.1, 0.1, 0.2, 0.3, 0.4};
	float outputArray_c[ARRAY_LENGTH];
	float outputArray_asm[ARRAY_LENGTH];
	
	
	
	
	// PART II: Comparison of Assembly and C Operations
	// Call assembly function and print output array
//	returnCode = call_asm(testArray, outputArray_asm, ARRAY_LENGTH, &kalmanState_asm);
//	if (returnCode != 0) { 
//		printf("Error in assembly function");
//		return -1;
//	}
//		
//	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_asm[%i] = %f\n", i, outputArray_asm[i]);
//	
	
	// Call C function to update kalman state and print output
	returnCode = Kalmannfilter_C(testArray, outputArray_c, &kalmanState_c, ARRAY_LENGTH);
	if (returnCode != 0) { 
		printf("Error in C function");
		return -1;
	}
	// Print output array from the c code
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_c[%i] = %f\n", i, outputArray_c[i]);
	// PART III Outputs - Our own implementation
	out_c = func(testArray, outputArray_c);
	printf("part 3 outputs\n");
	printf("diffarray\n");
	for (i=0; i < ARRAY_LENGTH; i++) printf("diffArray_c[%i] = %f\n", i, out_c.diffArr[i]);
	printf("mean: %.9f\n", out_c.meanDiff);
	printf("standard deviation: %.9f\n", out_c.standDevDiff);
	printf("mean: %.9f\n", out_c.meanDiff);
	for (i=0; i < ARRAY_LENGTH; i++) printf("convolArr[%i] = %f\n", i, out_c.convolArr[i]);

	
	// PART III Outputs - CMSIS-DSP implementation
	out_arm = funcArm(testArray, outputArray_c);
	
	
	
	
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
dsp_t func(float* inputArray, float* outputArray) {
	
	int i,k;
	float diffArray[ARRAY_LENGTH];
	float meanDiff = 0.0;
	float stanDev =  0.0;
	float sum_x = 0.0;
	float sum_y = 0.0;
	float sum_xy = 0.0;
	float sum_x2 = 0.0;
	float sum_y2 = 0.0;
	float correlation = 0.0;
	float tempSum;
	float convolArray[ARRAY_LENGTH];
	dsp_t analysisOut;
	
	// PART III: 4 Additional Operations
	//A: Subtraction of original data and filtered data
	
	for (i=0; i < ARRAY_LENGTH; i++) {
		diffArray[i] = outputArray[i] - inputArray[i];
		meanDiff += diffArray[i];
		//set output diff arrary
		analysisOut.diffArr[i]= diffArray[i];
	}
	//for (i=0; i < ARRAY_LENGTH; i++) printf("diffArray[%i] = %f\n", i, diffArray[i]);
	
	//B: Standard deviation: sigma = sqrt((1/N) sum(x_i-u)^2)	; also calculate average/mean of difference array
	
	meanDiff = meanDiff/ARRAY_LENGTH;	
	//printf("meanDiff: %f\n", meanDiff);
	
	for (i=0; i < ARRAY_LENGTH; i++) {
		stanDev += pow((diffArray[i] - meanDiff),2); 
	}
	stanDev = sqrt((1.0/ARRAY_LENGTH)*stanDev);
	//printf("standDev: %.9f\n", stanDev);
	
	//C: Calculate correlation coefficient between original and tracked vectors
	
	for (i=0; i < ARRAY_LENGTH; i++) {
		sum_xy += inputArray[i] * outputArray[i] ;
		sum_x += inputArray[i];
		sum_y += outputArray[i];
		sum_x2 += pow(inputArray[i],2);
		sum_y2 += pow(outputArray[i],2);
	}

	correlation = ( ARRAY_LENGTH*sum_xy - sum_x*sum_y )/( sqrt( ARRAY_LENGTH*sum_x2 - pow(sum_x,2) )*sqrt( ARRAY_LENGTH*sum_y2 - pow(sum_y,2) ) );
	//printf("correlation: %.9f\n", correlation);
	
	//D: Calculate convolution between the 2 vectors
	// y[n] = x[n]*h[n] = sum_{-5,5} x[k] * h[n-k]
	
	for(i=0; i<ARRAY_LENGTH; i++) {
		
		for(k=0; k<ARRAY_LENGTH;k++) {
			tempSum += inputArray[k] * outputArray[i - k];
			//printf("tempSum: %.9f\n", tempSum);
		}
	//	printf("___________\n");
		convolArray[i] = tempSum;
		analysisOut.convolArr[i] =  tempSum;
		tempSum = 0.0;
	}
	//for (i=0; i < ARRAY_LENGTH; i++) printf("convolArray[%i] = %0.9f\n", i, convolArray[i]);
	
	//analysisOut.diffArr[0]=1;
	analysisOut.meanDiff = meanDiff;
	analysisOut.standDevDiff = stanDev;
	analysisOut.correlation = correlation;
	
	
	return analysisOut;
}
dsp_t funcArm(float* inputArray, float* outputArray) {
	int i;
	//float diffArr[ARRAY_LENGTH];
	dsp_t analysisOut;
	arm_matrix_instance_f32 diffArr[ARRAY_LENGTH];
	
	arm_status arm_mat_subf32(&outputArray, &inputArray, &diffArr);
	//for (i=0; i < ARRAY_LENGTH; i++) printf("diffArray[%i] = %f\n", i, diffArr[i]);
	//analysisOut.diffArr = diffArr;
	
	return analysisOut;
}
