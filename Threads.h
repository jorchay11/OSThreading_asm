#ifndef THREADS_H_
#define THREADS_H_

/*
// P2OUT address is 0x40004C03
// Byte Offset -> 0x4000_4C03 - 0x4000_0000 = 0x0000_4C03
// Bit offset = Byte Offset X 32 + bit_num * 4
//            ->  (0x0000_4C03 x 32) + (1 * 4) = 622692 = 0x98064
//Bit Word Addr = alias base + bit offset
//            -> 0x4200_0000 + 622692 = 0x4209_8064
*/

#define GRN_LED (*((volatile unsigned char *) 0x42098064))

                        //variable declarations
extern volatile int count;
//comment OUT
extern unsigned int threadlock;

						// Function prototypes
void Thread0(void);
void Thread1(void);
void Thread2(void);




#endif 
