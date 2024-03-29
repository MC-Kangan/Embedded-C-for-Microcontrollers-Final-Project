#include <xc.h>
#include "interrupts.h"
#include "movement.h"
#include "timers.h"
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{   INTCONbits.GIE=1; 	//turn on interrupts globally
    INTCONbits.IPEN=1; 	//enable priority levels
    INTCONbits.GIEL=1;  // turn on peripheral interrupts
	PIE0bits.TMR0IE = 1; //enable timer
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/

void __interrupt(high_priority) HighISR()
{
    if(PIR0bits.TMR0IF){       
        TMR0H=0b00111100;            // Offset for the time to overflow exactly 0.1 seconds. More significant 8 bits in 0011110010101111
        TMR0L=0b10101111;            // Offset for the time to overflow exactly 0.1 seconds. Less significant 8 bits in 0011110010101111
        centisecond ++;              // when overflow occurs, add 1 to the global variable centisecond 
        PIR0bits.TMR0IF = 0;         // clear the flag
    }
}

