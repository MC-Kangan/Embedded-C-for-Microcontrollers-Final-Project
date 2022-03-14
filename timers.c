#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{   // timer that overflow for every 100ms
    T0CON1bits.T0CS=0b010;      // Fosc/4
    T0CON1bits.T0ASYNC=1;       // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b0101;   // 0.1/65535 = 4 * PS/64000000, PS = 24.4144 = 32
    T0CON0bits.T016BIT=1;       // 16bit mode	
	// In 1:32 pre-scale, Counting 50000 steps makes the timer overflow at exactly 0.1 second: 0.1/50000 = 4 * 32/64000000
    // Calculate the difference in steps: 65535 - 50000 = 15535
    // 15535 in binary(16bits) = 0011110010101111
    TMR0H=0b00111100;            // More significant 8 bits in 0011110010101111
    TMR0L=0b10101111;            // Less significant 8 bits in 0011110010101111
}
