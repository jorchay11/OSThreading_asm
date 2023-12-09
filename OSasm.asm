; ============================================================================
;   OSasm.asm: low-level OS commands, written in assembly
;   Runs on MSP432
;   A very simple real time operating system with minimal features.
; ============================================================================

        .thumb
        .text
        .align 2


        .global  RunPt            		; currently running thread
        .global  OS_DisableInterrupts
        .global  OS_EnableInterrupts
        .global  StartOS
        .global  SysTick_Handler


OS_DisableInterrupts:  .asmfunc			; Complete this
        
	CPSID	I
	BX	LR
    .endasmfunc


OS_EnableInterrupts:  .asmfunc			; Complete this
        
	CPSIE	I
	BX 	LR
    .endasmfunc


RunPtAddr .field RunPt,32

SysTick_Handler:  .asmfunc  ; 1) Handler automatically saves R0-R3,R12,LR,PC,PSR on stack upon entry
	;B OS_DisableInterrupts	; 2) Disable interrupt mechanism to prevent interrupts from happening during switch
	CPSID 	I
	PUSH	{R4,R5,R6,R7,R8,R9,R10,R11} 	; 3) Save remaining regs r4-11 on stack (of current thread)
	LDR R0,	RunPtAddr						; 4) Load R0 with the address of RunPt, current thread
	LDR R1, [R0]							; 5) Load R1 with the value of RunPt
	STR SP,	[R1]							; 6) Save current SP into TCB structure
	LDR R1,	[R1, #4]						; 7) Load R1 with the value of RunPt->next
	STR R1, [R0]							; 8) Store it in RunPt (so it points to next thread)
	LDR SP, [R1]							; 9) Load new thread SP; SP = RunPt->sp;
	POP		{R4,R5,R6,R7,R8,R9,R10,R11}  	;10) Restore regs r4-11 (next thread)
	CPSIE 	I
	;B OS_EnableInterrupts	;11) Enable interrupt mechanism
	BX 		LR									;12) Return to calling function (R0-R3,R12,LR,PC,PSR are automatically restored upon exit
    .endasmfunc


StartOS:  .asmfunc			    			; 1) Kickstarts the process with 1st thread
	LDR 	R0,	RunPtAddr		    		; 2) Load r0 with the address of runpt, current thread
	LDR 	R2, [R0]						; 3) Load R2 with the value of RunPt
	LDR 	SP, [R2]			    		; 4) Load SP with new thread SP; SP = RunPt->stackPointer;
	POP 	{R4,R5,R6,R7,R8,R9,R10,R11} 	; 5) Restore regs r4-11
	POP 	{R0,R1,R2,R3}	   				; 6) Restore regs r0-3
	POP 	{R12}				    		; 7) Restore reg r12
	POP 	{LR}							; 8) Restore LR and discard LR from initial stack
	POP 	{LR}				    		; 9) Restore return address and store it in LR (start location)----
	POP 	{R1}							;10) Restore PSR into R1 and discard PSR
	CPSIE 	I
	;B OS_EnableInterrupts	   				;11) Enable interrupt mechanism at processor level
	BX 		LR					    		;12) Return to calling function whcih would start the first thread
    .endasmfunc

    .end
