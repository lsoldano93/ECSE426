; Luke Soldano, Xavier Agostini (C) 2016
; McGill University, ECSE 426, Lab One
; Assembly code to implemenet a Kalmann filter

	AREA fn, CODE, READONLY
	IMPORT Kalmanfilter_asm
	IMPORT kalmanStruct
	EXPORT testBench_asm
testBench_asm
; Push return link onto stack
  PUSH {R14}
 
; Initialize q,r,x,p,k

	VLDR.f32 S4, =0.0 ; float q
	VLDR.f32 S5, =0.1 ; float r 
	VLDR.f32 S6, =0.0 ; float x
	VLDR.f32 S7, =0.1 ; float p
	VLDR.f32 S8, =0.0 ; float k
	LDR R3, =kalmanStruct ;load address
	VSTM R3, {S4-S8}
 
; Run kalman filter
	LDR     R0, =Kalmanfilter_asm
	BLX     R0

; Pop return link off of stack
	POP {R14}

	ALIGN
testArray DCB 1,2,3,4,5		
; Return from branch
	BX LR
	END