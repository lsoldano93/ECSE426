; Luke Soldano, Xavier Agostini (C) 2016
; McGill University, ECSE 426, Lab One
; Assembly code to implemenet a Kalmann filter

	AREA fn, CODE, READONLY
	IMPORT Kalmanfilter_asm
	IMPORT dummyInput
	IMPORT dummyOutput
	IMPORT dummyStruct
	EXPORT testBench_asm
testBench_asm

; Push return link onto stack
  PUSH {R14}

; Give values to input array for processing **(Change these values to test output)
	VLDR.f32 S4, =0.1
	VLDR.f32 S5, =0.1 
	VLDR.f32 S6, =0.2 
	VLDR.f32 S7, =0.3 
	VLDR.f32 S8, =0.4
	LDR R0, =dummyInput ;load address of dummy struct
	VSTM R0, {S4-S8} ; R0 acts as pointer to input array 

; Allocate memory for output array for values to be placed in by filter
	VLDR.f32 S4, =0.0 
	VLDR.f32 S5, =0.0
	VLDR.f32 S6, =0.0
	VLDR.f32 S7, =0.0 
	VLDR.f32 S8, =0.0
	LDR R1, =dummyOutput
	VSTM R1, {S4-S8}

; Update R2 to pass array length
	MOV R2, #5

; Initialize q,r,x,p,k (Pointer to Kalman Struct)
	VLDR.f32 S4, =0.1 ; float q
	VLDR.f32 S5, =0.1 ; float r 
	VLDR.f32 S6, =0.0 ; float x
	VLDR.f32 S7, =0.0 ; float p
	VLDR.f32 S8, =0.0 ; float k
	LDR R3, =dummyStruct ;load address of dummy struct
	VSTM R3, {S4-S8} ; R3 acts as a pointer to the struct 

; 1: Pointer to input data array
; 2: Pointer to filtered data array
; 3: Array length

; Run kalman filter
	LDR     R12, =Kalmanfilter_asm
	BLX     R12

; Pop return link off of stack
	POP {R14}

	END