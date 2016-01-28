; Luke Soldano, Xavier Agostini (C) 2016
; McGill University, ECSE 426, Lab One
; Assembly code to implemenet a Kalmann filter

	AREA fn, CODE, READONLY
	EXPORT Kalmanfilter_asm
Kalmanfilter_asm

; ***Begin code for Kalmann filter***

; Filter takes four input paramaters
; 1. A pointer to the input data array
; 2. A pointer to the filtered data array
; 3. The length of the arrays
; 4. A pointer to the Kalmann filter struct/state

; For the function return value, registers R0/R1 & S0/S1 
; are used for integer and floating-point results respectively

; Filter will hold its state as a quintuple (q,r,x,p,k) - all fp #'s
; Filter will load these values into registers (S4,S5,S6,S7,S8)

; TODO: Load input array values into proper fp registers
	VLDR.f32 S4, R0() ; How to access array at index?
	VLDR.f32 S5, R0()
	VLDR.f32 S6, R0() 
	VLDR.f32 S7, R0()
	VLDR.f32 S8, R0() 

	
; Return from branch
	BX LR
	END