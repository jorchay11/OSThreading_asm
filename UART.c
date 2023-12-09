// ===== Include all the appropriate HEADER FILES =====

#include "UART.h"
#include <msp.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// ===== This function configures UART0 =====
void UART0_Init(void)
{
    EUSCI_A0 -> CTLW0 |= 1;       // Put in RESET mode for config
    EUSCI_A0 -> MCTLW = 0;        // Disable oversampling
    EUSCI_A0 -> BRW = 312;        // 3 MHz / 9600 = 312
    EUSCI_A0 -> CTLW0 = 0x0081;   // 1 stop bit, no parity, SMCLK, 8-bit data
    P1 -> SEL0 |= 0x0C;           // P1.3 and P1.2 for UART
    P1 -> SEL1 &= ~0x0C;
    EUSCI_A0 -> CTLW0 &= ~1;      // Take UART out of RESET mode
}


// ===== This function sends a character to UART terminal =====
int UART_Send(char a)
{
    while(!(EUSCI_A0 -> IFG & 0x02));     // Wait for transmit buffer empty
    EUSCI_A0 -> TXBUF = a;                // Send a character


    return (int)a;                        //return char sent
}


// ===== This function transmits a string =====
int UART_MSend(char *a)
{
    unsigned int len = strlen(a);

    while(*a != '\0')
    {
      while(!(EUSCI_A0 -> IFG & 0x02));     // Wait for transmit buffer empty
      EUSCI_A0 -> TXBUF = *a++;             // Send a character

    }

    return len;                             //return number of characters sent
}


// ===== This function receives a character =====
char UART_Receive(void)
{
    char a;

     while(!(EUSCI_A0 -> IFG & 0x01));     // Wait for receive buffer full
     a = EUSCI_A0 -> RXBUF;                // Receive a character

     return a;                             //return char received
}
