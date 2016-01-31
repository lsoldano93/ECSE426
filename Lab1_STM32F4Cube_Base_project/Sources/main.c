#include <stdio.h>
#include "arm_math.h"
//#include "test.h"

#define ARRAY_LENGTH 5

// Keil solution for printf viewing http://www.keil.com/support/man/docs/ulink2/ulink2_trace_itm_viewer.htm
//#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
//#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
//#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

//#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
//#define TRCENA          0x01000000

//struct __FILE { int handle; /* Add whatever needed */ };
//FILE __stdout;
//FILE __stdin;
//
//int fputc(int ch, FILE *f) {
//  if (DEMCR & TRCENA) {
//    while (ITM_Port32(0) == 0);
//    ITM_Port8(0) = ch;
//  }
//  return(ch);
//}
// End Keil solution code *********************************************************************************

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
	
	int i, returnCode;
	
	float testArray[ARRAY_LENGTH] = {0.1, 0.1, 0.2, 0.3, 0.4};
	float outputArray_c[ARRAY_LENGTH];
	float outputArray_asm[ARRAY_LENGTH];
	
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
	
	// Call assembly function to update kalman state with input
	//Kalmanfilter_asm(testArray, outputArray_asm, ARRAY_LENGTH, kalmanState_asm);
	
	//for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_asm[%i] = %f", i, outputArray_asm[i]);
	
	// Call C function to update kalman state with input
	returnCode = Kalmannfilter_C(testArray, outputArray_c, kalmanState_c, ARRAY_LENGTH);
	
	for (i=0; i < ARRAY_LENGTH; i++) printf("outputArray_c[%i] = %f", i, outputArray_c[i]);
	
	// TODO: subtract output arrays to analyze difference in values
	
	return returnCode;
}

int Kalmannfilter_C(float* InputArray, float* OutputArray, kalman_t* kstate, int Length){
	
	int i;
	
	for (i=0; i < Length; i++){
		kstate->p = kstate->p + kstate->q;
		kstate->k = kstate->p / (kstate->p + kstate->r);
		kstate->x = kstate->x + kstate->k * (InputArray[0] - kstate->x);
		kstate->p = (1 - kstate->k) * kstate->p;
		OutputArray[i] = kstate->x;
	}
	
	return 0;
}
