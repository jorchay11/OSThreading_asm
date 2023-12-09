// ===== Include appropriate header files =====

#include <msp.h>
#include "UART.h"
#include "Threads.h"
#include "OS.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ===== This is the kernel that will setup everything and run the scheduler ======

//REROUTE SVC HANDLER TO GO TO SYSTICK BY modifying the startup.c file
//AND interruptVectors(Void) in startup.c

#define TIMESLICE  6000         // Thread context switch time in system time units
                                // 2ms


// Code below is the interface to the C standard I/O library
// All the I/O are directed to the console which is UART0
struct __FILE { int handle; };
FILE __stdin = {0};
FILE __stdout = {1};
FILE __stderr = {2};

// ====== Called by C library console / file output =======
int fputc(int c, FILE *f)
{
    
    int temp;

    temp = UART_Send((char)c);       // Write character to console

    return temp;                     // Return the character sent
}

// ====== For using printf() with format specifiers ======
int fputs(const char *_ptr, register FILE *_fp)
{
    
    unsigned int len;

    len = UART_MSend(_ptr);     // Send a string

    return len;                 // Return the length of the string
}


//initialize green LED pin
void GREEN_LED()
{
    P2->SEL0 &= ~BIT1;
    P2->SEL1 &= ~BIT1;  //P2.1 set to GPIO
    P2->DIR |= BIT1;    //P2.1 direction set to output
    P2->OUT &= ~BIT1;   //P2.1 set to off or as 0
}

// ====== GPIO_Init() =======
void GPIO_Init(void)
{
    // Initialize GPIO periperals needed in this project
   GREEN_LED();           //init GREEN onboard LED

}

// ======== Function to initialize LOCK (you need to modify) =============
void Lock_Init(unsigned *lock)
{
    *lock = 1;
}

// ======== Function to release LOCK (you need to modify) ===========
void Lock_Release(unsigned *lock)
{
	*lock = 1;
}

//halts the watchdog timer
void Watchdog_halt()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
}


// ======= MAIN =========
int main(void)
{
	Watchdog_halt();            // stop watchdog timer
	Lock_Init(&threadlock);		// Initialize the global thread lock
	OS_Init();                  // Initialize OS
	UART0_Init();               // Initialize UART0 peripheral
	GPIO_Init();                // Initialize GPIO peripheral

	if(OS_AddThreads(Thread0, Thread1, Thread2))        // Add Threads to the list
	    OS_Launch(TIMESLICE);                           // Launch OS


	return 0;                   // This never executes

}
