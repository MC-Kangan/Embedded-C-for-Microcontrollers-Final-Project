#include <xc.h>
#include <stdio.h>
#include "color.h"
#include "dc_motor.h"
#include "serial.h"
#include "movement.h"

void action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR)
{   fullSpeedAhead(mL,mR);
    if (color != 0){stop(mL,mR);}
}
void test_action (struct DC_motor *mL, struct DC_motor *mR)
{   fullSpeedAhead_test(mL,mR);
    turnLeft(mL,mR,90);
    fullSpeedAhead_test(mL,mR);
    turnLeft(mL,mR,90);
    fullSpeedAhead_test(mL,mR);
    turnLeft(mL,mR,180);
    fullSpeedAhead_test(mL,mR);
    turnRight(mL,mR,90);
    fullSpeedAhead_test(mL,mR);
    turnRight(mL,mR,90);
    fullSpeedAhead_test(mL,mR);
}
void pin_init(void)
{   TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin     
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
    
    TRISFbits.TRISF6 = 0; //output on RF6 (BAT-VERSE), detects voltage of motor
    LATFbits.LATF6 = 0; //output on BAT-VERSE set on (power) 
    
    TRISGbits.TRISG1 = 0; //output on RG1 (LED_R)
    TRISFbits.TRISF7 = 0; //output on RF7 (LED_G)
    TRISAbits.TRISA4 = 0; //output on RA4 (LED_B)
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_G set on (power)
    LATAbits.LATA4 = 1; // output LED_B set on (power)
}

void goback(struct DC_motor *mL, struct DC_motor *mR)
{
    turnLeft(mL, mR, 180);   
}