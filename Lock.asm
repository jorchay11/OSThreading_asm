; ============================================================================
;   lock.asm: lock_acquire function, written in assembly
;   Runs on MSP432
; ============================================================================
	;.syntax unified
		.thumb
		.text
		.align 2

    	;.type lock_acquire, function
		.global Lock_Acquire

Lock_Acquire:	.asmfunc
	MOV R1, #0
		
	LDREX R2, [R0]
	CMP R2,R1
	ITT NE
	STREXNE R2, R1, [R0]

	CMPNE R2, #1
	BEQ L1

	MOV R0, #1
	BX LR
		
L1:
	CLREX
	;returns 0 if lock was aquired
	MOV R0, #0
	BX LR

	.endasmfunc

    .end
