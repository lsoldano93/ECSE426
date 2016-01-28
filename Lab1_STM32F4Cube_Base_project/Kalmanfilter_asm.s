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

; Initialize q,r,x,p,k
; Load kalman state values into proper fp registers
	VLDR.f32 S4, [R3, #0] ; float q
	VLDR.f32 S5, [R3, #4] ; float r 
	VLDR.f32 S6, [R3, #8] ; float x
	VLDR.f32 S7, [R3, #12] ; float p
	VLDR.f32 S8, [R3, #16] ; float k
	
; Load input array
;	VLDR.f32 S9, [R0, #0] ; load first index of input array, this is the measurement value
	VMOV.f32 S9, S6 ; load first index of input array, this is the measurement value
	
	
; Find values of p and k
	VADD.f32 S7, S7, S4 ; p = p + q 
	VADD.f32 S10, S7, S5 ; p + r 
	VDIV.f32 S8, S7, S10 ; k = p / (p + r)
	
; Check for division overflow
	; LDR	ASPR, FPSCR
	; TODO: Perform check
	
; Find value of x
	VSUB.f32 S11, S9, S6 ; (measurement - x)
	VMUL.f32 S12, S8, S11 ; k * (measurement - x)
	
; Check for multiplication overflow
	; LDR	ASPR, FPSCR
	; TODO: Perform check
	
; Finish operation on x
	VADD.f32 S6, S6, S12 ; x = x + k *(measurement - x)

; Find value of p
	VLDR.f32 S11, =1.0
	VSUB.f32 S10, S11, S8 ; (1 - k)
	VMUL.f32 S7, S10, S7 ; p = (1 - k) * p
	
; Check for multiplication overflow
	; LDR	ASPR, FPSCR
	; TODO: Perform check
	
; Load x into return register
	VMOV.f32 S0, S6
	
; Return from branch
	BX LR
	END
		
