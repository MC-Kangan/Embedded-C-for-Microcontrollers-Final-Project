#include <xc.h>
#include <stdio.h>
#include "color.h"
#include "dc_motor.h"
#include "serial.h"
#include "movement.h"

//void movement(struct DC_motor *mL, struct DC_motor *mR)
//{
//    turnLeft(mL, mR);
//    __delay_ms(500);
//    stop(mL, mR);
//    __delay_ms(500);
//}
void test_movement (struct DC_motor *mL, struct DC_motor *mR)
{
    turnRight(mL, mR, 90);
    stop(mL, mR);
}
void calibration_init(void)
{   TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin     
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
}

void goback(struct DC_motor *mL, struct DC_motor *mR)
{
    turnLeft(mL, mR, 180);   
}