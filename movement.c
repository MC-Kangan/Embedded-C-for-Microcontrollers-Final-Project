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
    turnLeft(mL, mR, 90);
    stop(mL, mR);
}