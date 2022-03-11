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
}

void __interrupt(low_priority) LowISR()
{
    if(PIR0bits.TMR0IF){       
        second ++;                  // when overflow occurs, 1 second passes
        PIR0bits.TMR0IF = 0;             // clear the flag
    }
}

