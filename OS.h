#ifndef OS_H_
#define OS_H_

// fill these depending on your clock
#define TIME_1MS    3000
#define TIME_2MS    6000  		// This is what you should pass as the Timeslice value

// SysTick Control and Status Register (STCSR)
#define STCSR_COUNT_FG  (0x0100)
#define STCSR_CLKSRC    (0x0004)
#define STCSR_INT_EN    (0x0002)
#define STCSR_EN        (0x0001)

//extern int32_t StartCritical(void) __attribute__((naked));

						// Function protypes and variable declarations
void OS_Init(void);
void SetInitialStack(int i);
int OS_AddThreads(void(*Thread0)(void), void(*Thread1)(void), void(*Thread2)(void));
void OS_Launch(uint32_t theTimeSlice);
///////////////////////////////////////////////////
//???
int32_t StartCritical(void);
void EndCritical(int32_t primask);
void yield(void);

extern void OS_DisableInterrupts(void);
extern void OS_EnableInterrupts(void);
extern void StartOS(void);
extern void SysTick_Handler(void);

#endif
