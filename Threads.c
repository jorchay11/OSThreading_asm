// ===== Include appropriate header files =====

#include <msp.h>
#include "Threads.h"
#include "UART.h"
#include "OS.h"
//#include "Kernel.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>



volatile int count = 0;
//extern unsigned threadlock;
unsigned threadlock;

extern void Lock_Release(unsigned *lock);
extern unsigned Lock_Acquire(unsigned *lock);
extern void Lock_Init(unsigned *lock);

// These are the user-space threads. Note that they are completely oblivious
// to the technical concerns of the scheduler. The only interface to the
// scheduler is the single function yield() 

// ===== Thread 0 =====         DONE
// ===== Responsible for toggling on-board GREEN RGB LED infinitely ====
// ===== Use BIT-BANDING approach to reference the LED =====
// ===== Control taken away pre-emtively by the KERNEL =====
void Thread0(void)
{
  while(1)
  {
      //bit band alias extrapolated above define
      //it is bit 0 because this is the 0th bit of the OUT band we aliased
      //GRN_LED ^= 0x01;
      GRN_LED ^= BIT0;
  }
  
}


// ===== Thread 1 =====     DONE
// ===== Responsible for printing message to the terminal window =====
// ===== Locks need to be implemented for this thread =====
// ===== Control needs to be given up voluntarily by this thread using yield() =====
void Thread1(void)
{
  
  while(1)
  {
   if(Lock_Acquire(&threadlock))		// Try to acquire lock and if successful
   {
    
    printf("\n\rEntered");
    yield();					// Give up control voluntarily (context switch "interrupt")

    count = count + 1;          //-----------------------------------------added

    printf(" Thread 1 -- pass %d", count);
	Lock_Release(&threadlock);					// Release lock
   }
   yield();                		// Give up control voluntarily (context switch "interrupt")
  }
}


// ===== Thread 2 =====     DONE
// ===== Responsible for printing message to the terminal window =====
// ===== Locks need to be implemented for this thread =====
// ===== Control needs to be given up voluntarily by this thread using yield() =====
void Thread2(void)
{
  while(1)
  {
   if(Lock_Acquire(&threadlock))		// Try to acquire lock and if successful
   {
    printf("\n\rEntered");
    yield();					// Give up control voluntarily (context switch "interrupt")

    printf(" Thread 2");
    Lock_Release(&threadlock);			// Release lock
   }
   yield();                		// Give up control voluntarily (context switch "interrupt")
  }
}

