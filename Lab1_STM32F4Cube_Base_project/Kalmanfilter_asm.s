; Luke Soldano, Xavier Agostini (C) 2016
; McGill University, ECSE 426, Lab One
; Assembly code to implemenet a Kalmann filter

	AREA fn, CODE, READONLY
	EXPORT Kalmanfilter_asm
Kalmanfilter_asm

; Filter takes four input paramaters
; 1. A pointer to the input data array
; 2. A pointer to the filtered data array
; 3. The length of the arrays
; 4. A pointer to the Kalmann filter struct/state

; For the function return value, registers R0/R1 & S0/S1 
; are used for integer and floating-point results respectively

; Register inputs (Input array, output array, array length, kalman struct address)

; Filter will hold its state as a quintuple (q,r,x,p,k) - all fp #'s
; Filter will load these values into registers (S4,S5,S6,S7,S8)

; Initialize q,r,x,p,k
; Load kalman state values into proper fp registers
	VLDR.f32 S4, [R3, #0] ; float q
	VLDR.f32 S5, [R3, #4] ; float r 
	VLDR.f32 S6, [R3, #8] ; float x
	VLDR.f32 S7, [R3, #12] ; float p
	VLDR.f32 S8, [R3, #16] ; float k

; Initialize loop counter
	MOV R4, #0

; Start loop
LOOP

; Load input array
	VLDR.f32 S9, [R0, #0] ; load proper index of input array to S9 (measurement value)	
	
; Find values of p and k
	VADD.f32 S7, S7, S4 ; p = p + q 
	VADD.f32 S10, S7, S5 ; p + r 
	
; ERROR CHECK ONE: Check for zero (not a number) in divisor
	VCMP.f32 S10, #0
	VMRS APSR_nzcv, FPSCR
	BEQ ERROR_OUT
	
; Finish operation on k
	VDIV.f32 S8, S7, S10 ; k = p / (p + r)
	
; Find value of x
	VSUB.f32 S11, S9, S6 ; (measurement - x)
	VMUL.f32 S12, S8, S11 ; k * (measurement - x)
	
; Finish operation on x
	VADD.f32 S6, S6, S12 ; x = x + k * (measurement - x)

; ERROR CHECK TWO: Check if x induced overflow (NOT POSSIBLE FOR VALUE TO OVERFLOW)
	;VMRS APSR_nzcv, FPSCR
	;BVS ERROR_OUT

; Find value of p
	VLDR.f32 S11, =1.0
	VSUB.f32 S10, S11, S8 ; (1 - k)
	VMUL.f32 S7, S10, S7 ; p = (1 - k) * p

; ERROR CHECK THREE: Check if p induced overflow (NOT POSSIBLE FOR VALUE TO OVERFLOW)
	;VMRS APSR_nzcv, FPSCR
	;BVS ERROR_OUT

; Update output array and struct values
	VSTM R3, {S4-S8}
	VSTM R1, S6 
	
; Increment R0 & R1 addresses to return proper pointer for next iteration
	ADD R0, R0, #4
	ADD R1, R1, #4
	
; Determine whether or not loop should continue
	ADD R4, R4, #1
	CMP R4, R2
	BLT LOOP

; Load 0 into return register to indicate no error
	MOV R0, #0

; Return from branch
	BX LR
	
; Function called when overflow or other error detected
ERROR_OUT

; Load non-zero value into return register to indicate error
	MOV R0, #-1
	BX LR
	
	END
		
