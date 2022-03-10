#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010;      // Fosc/4
    T0CON1bits.T0ASYNC=1;       // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1000;   // 1/65535 = 4 * PS/64000000, PS = 244.144 = 256    
    T0CON0bits.T016BIT=1;       // 16bit mode	
	// In 1:256 pre-scale, Counting 62500 steps makes the timer overflow at exactly 1 second: 1/62500 = 4 * 256/64000000
    // Calculate the difference in steps: 65535 - 62500 = 3035
    // 3035 in binary(16bits) = 0000101111011011
    TMR0H=0b00001011;            // More significant 8 bits in 0000101111011011
    TMR0L=0b11011011;            // Less significant 8 bits in 0000101111011011
    T0CON0bits.T0EN=1;	//start the timer
}
