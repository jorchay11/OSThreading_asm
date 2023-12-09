// ===== Include appropriate header files =====
#include <msp.h>
#include "OS.h"
#include "UART.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ===== Function prototypes of functions written in OSasm.asm =====
//write these as ASM prototypes from OSasm.asm
void OS_DisableInterrupts(void);
void OS_EnableInterrupts(void);
void SysTick_Handler(void);
void StartOS(void);

#define NUMTHREADS  3             // Maximum number of threads
#define STACKSIZE   300           // Number of 32-bit words in stack (You will have to keep increasing intelligently if code does not work)

// ===== Structure to store state of thread =====
struct tcb
{
  int32_t *sp;                      // Pointer to stack (valid for threads not running)
  struct tcb *next;                 // Linked-list pointer
};
typedef struct tcb tcbType;

tcbType tcbs[NUMTHREADS];			    // Thread table
tcbType *RunPt;						    // Pointer to pointto the next thread
int32_t Stacks[NUMTHREADS][STACKSIZE];	// 2-dimensional arry implemented as STACK

// ===== OS_Init =====
// Initialize operating system, disable interrupts until OS_Launch
// Initialize OS controlled I/O: SysTick, 3 MHz crystal
void OS_Init(void)
{
  OS_DisableInterrupts();       //in template

  SysTick->CTRL  &= ~BIT0;		// Disable SysTick during setup (register approach)
  SysTick->VAL    = 0;			// Any write to current clears it (register approach)

  SCB -> SHP[1] = (SCB -> SHP[1] & 0x00FFFFFF)|0xE0000000;          // Set priority 7 so that it allows threads to run
}


// ===== Sets default values in STACK ======    FIX?
// Inputs: i is the threadnumber
void SetInitialStack(int i)
{   //0x2000_FFFF end of RAM
    //SP mod 4 = 0, must be aligned at all times
  tcbs[i].sp = &Stacks[i][STACKSIZE-16]; // thread stack pointer

  Stacks[i][STACKSIZE-1] = 0x01000000;   // XPSR (store appropriate initial value) 	-- Saved by Exception
  Stacks[i][STACKSIZE-3] = 0x20141414;   // R14 (store appropriate initial value)
  Stacks[i][STACKSIZE-4] = 0x20121212;   // R12 (store appropriate initial value)
  Stacks[i][STACKSIZE-5] = 0x20303030;   // R3 (store appropriate initial value)
  Stacks[i][STACKSIZE-6] = 0x20202020;   // R2 (store appropriate initial value)
  Stacks[i][STACKSIZE-7] = 0x20101010;   // R1 (store appropriate initial value)
  Stacks[i][STACKSIZE-8] = 0x20000000;   // R0 (store appropriate initial value)	-- Saved by Exception
  Stacks[i][STACKSIZE-9] = 0x20111111;   // R11 (store appropriate initial value)	-- Saved by you
  Stacks[i][STACKSIZE-10] = 0x21010101;   // R10 (store appropriate initial value)
  Stacks[i][STACKSIZE-11] = 0x20909090;   // R9 (store appropriate initial value)
  Stacks[i][STACKSIZE-12] = 0x20808080;   // R8 (store appropriate initial value)
  Stacks[i][STACKSIZE-13] = 0x20707070;   // R7 (store appropriate initial value)
  Stacks[i][STACKSIZE-14] = 0x20606060;   // R6 (store appropriate initial value)
  Stacks[i][STACKSIZE-15] = 0x20505050;   // R5 (store appropriate initial value)
  Stacks[i][STACKSIZE-16] = 0x20404040;   // R4 (store appropriate initial value)	-- Saved by you
}


// ====== OS_AddThread ======       DONE
// Add three foreground threads to the scheduler in a Round-Robin fashion
// Inputs: three pointers to a void/void foreground tasks
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThreads(void(*Thread0)(void), void(*Thread1)(void), void(*Thread2)(void))
{
  int32_t status;

  status = StartCritical();                              //call function start crit

  tcbs[0].next = &tcbs[1];          //make sure *next is linked to next thread			   // next pointer of Thread 0 points to Thread 1 structure
  tcbs[1].next = &tcbs[2];			// next pointer of Thread 1 points to Thread 2 structure
  tcbs[2].next = &tcbs[0];	        // next pointer of Thread 2 points to Thread 0 structure

  if(*Thread0)
  {
  SetInitialStack(0);    // 1: Set the default values in stack
  Stacks[0][STACKSIZE-2] = (uint32_t)Thread0; // 2: Make ReturnAddress stored on stack to point to Thread 0
  }
  else
      return 0;         //unsuccessful

  if(*Thread1)
  {
  SetInitialStack(1);                     // 1: Set the default values in stack
  Stacks[1][STACKSIZE-2] = (uint32_t)Thread1;    // 2: Make ReturnAddress stored on stack to point to Thread 1
  }
  else
      return 0;

  if(*Thread2)
  {
  SetInitialStack(2);                     // 1: Set the default values in stack
  Stacks[2][STACKSIZE-2] = (uint32_t)Thread2;    // 2: Make ReturnAddress stored on stack to point to Thread 2
  }
  else
      return 0;
  
  RunPt = &tcbs[0];       // Make RunPt point to Thread 0 so it will run first

  EndCritical(status);    // Function call to end the critical section

  return 1;               // successful
}


// ===== OS_Launch ======       DONE
// Start the scheduler, Enable interrupts
// Inputs: number of 333.33ms clock cycles that corresponds to 2 ms
//         (maximum of 24 bits)
void OS_Launch(uint32_t theTimeSlice)
{
  SysTick->LOAD = theTimeSlice - 1;                                 // Load the reload value for Systick Timer
  SysTick->CTRL   = (STCSR_CLKSRC | STCSR_INT_EN | STCSR_EN); 	    // Enable the Timer, Use core clock and arm interrupt
  StartOS();                                                        // start on the first task
}


// ====== StartCritical ======      DONE
// Make a copy of previous I bit, disable interrupts
// Outputs: previous I bit
extern int32_t StartCritical(void) __attribute__((naked));
int32_t StartCritical(void)
{
    asm volatile(" MRS R0, PRIMASK");
    asm volatile(" CPSID I");    // Disable interrupt mechanism in assembly
    asm volatile(" BX LR");      // Return to the calling function
}


// ====== EndCritical ======        DONE
// Using the copy of previous I bit, restore I bit to previous value
// Inputs:  previous I bit
extern void EndCritical(int32_t primask) __attribute__((naked));
void EndCritical(int32_t primask)
{
    asm volatile(" MRS R0, PRIMASK");
    asm volatile(" BX LR");      // Return to the calling function
}


// ====== This function (written in assembly) switches to handler mode. (privileged access) =======
extern void yield(void) __attribute__((naked));
void yield(void)
{
    asm volatile(" SVC #123");				//call an SVC structure, have to call handler to switch
}


//-----------------------------------------------------------
//=======================           DONE
extern void unpriv_to_priv(void) __attribute__((naked));
void unpriv_to_priv(void)
{
    asm volatile(" MRS R0, CONTROL");
    asm volatile(" AND R0, R0, #0xFE");
    asm volatile(" MSR CONTROL, R0");
    asm volatile(" BX LR");
}


