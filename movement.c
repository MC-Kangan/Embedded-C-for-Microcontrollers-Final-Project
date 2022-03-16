 #include <xc.h>
#include "color.h"
#include "dc_motor.h"
#include "movement.h"


void short_burst(struct DC_motor *mL, struct DC_motor *mR)
{// a short burst movement for the buggy: moving ahead for 400ms and stop
    fullSpeedAhead(mL,mR);
    __delay_ms(400);
    stop(mL,mR);
}

void turning_action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR)
{    if (color != 0){                   // if color is black, no action will be taken
        if (color == 1){                // if color is red
            fullSpeedBack(mL,mR,1);     // Reverse moving for a small distance to leave space for turning
            turn45(mL,mR,2,2);          // Turn Right 90 degrees  
            memory[array_index] = 1;    // Record the color=1(red) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 2){                // if color is green
            fullSpeedBack(mL,mR,1);     // Reverse moving for a small distance to leave space for turning
            turn45(mL,mR,2,1);          // Turn Left 90 degrees 
            memory[array_index] = 2;    // Record the color=2(green) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 3){                // if color is blue
            fullSpeedBack(mL,mR,1);     // Reverse moving for a small distance to leave space for turning
            turn45(mL,mR,4,1);          // Turn 180 degrees
            memory[array_index] = 3;    // Record the color=3(blue) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 4){                // if color is yellow
            fullSpeedBack(mL,mR,3);     // Reverse moving for a large distance (one square)
            turn45(mL,mR,2,2);          // Turn Right 90 degrees          
            memory[array_index] = 4;    // Record the color=4(yellow) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 5){                // if color is pink
            fullSpeedBack(mL,mR,3);     // Reverse moving for a large distance (one square)
            turn45(mL,mR,2,1);          // Turn Left 90 degrees
            memory[array_index] = 5;    // Record the color=5(pink) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 6){                // if color is orange
            fullSpeedBack(mL,mR,1);     // Reverse moving for a small distance to leave space for turning
            turn45(mL,mR,3,2);          // Turn Right 135 degrees
            memory[array_index] = 6;    // Record the color=6(orange) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 7){                // if color is light blue
            fullSpeedBack(mL,mR,1);     // Reverse moving for a small distance to leave space for turning
            turn45(mL,mR,3,1);          // Turn Left 135 degrees 
            memory[array_index] = 7;    // Record the color=7(light blue) in memory for the current array index position
            array_index++;              // increase array index position by 1 for next recording
        }
        if (color == 8){                // if color is white
            toggle_light(2,1);          // toggle the buggy back light to show it's going back to the starting point
            goback(mL,mR);              // Call goback function to make the buggy back to the starting point
        }
    }
}

void goback(struct DC_motor *mL, struct DC_motor *mR)
{   fullSpeedBack(mL,mR,1);     // Reverse moving for a small distance to leave space for turning
    turn45(mL,mR,4,1);          // Turn 180 for a small distance to leave space for turning
    fullSpeedBack(mL,mR,2);     // Reverse moving for a medium distance so the buggy can auto calibrate the position. 
    array_index--;              // Reduce the array index by 1 for reading the memory data later.
    while(array_index >= 0){
        fullSpeedAhead(mL,mR);
        for (unsigned int i=0; i<memory[array_index]; i++) {__delay_ms(100);}
        stop(mL,mR);
        if (array_index == 0){break;}   
        array_index--;                  // Reduce the array index by 1 for reading the memory data later.
        if (memory[array_index] == 1){turn45(mL,mR,2,1);fullSpeedBack(mL,mR,2);array_index--;}
        else if (memory[array_index] == 2){turn45(mL,mR,2,2);fullSpeedBack(mL,mR,2);array_index--;}
        else if (memory[array_index] == 3){turn45(mL,mR,4,2);fullSpeedBack(mL,mR,2);array_index--;}
        else if (memory[array_index] == 4){turn45(mL,mR,2,1);fullSpeedBack(mL,mR,3);array_index--;}
        else if (memory[array_index] == 5){turn45(mL,mR,2,2);fullSpeedBack(mL,mR,3);array_index--;}
        else if (memory[array_index] == 6){turn45(mL,mR,3,1);fullSpeedBack(mL,mR,2);array_index--;}
        else if (memory[array_index] == 7){turn45(mL,mR,3,2);fullSpeedBack(mL,mR,2);array_index--;}
    }
    LATDbits.LATD7 = 1;     // turn on RD7 to indicate users to press RF2
    while(PORTFbits.RF2){   // wait RF2 to be pressed, otherwise the buggy will stop forever
        stop(mL,mR); 
        if (!PORTFbits.RF2){LATDbits.LATD7 = 0;break;}      // if RF2 is pressed, turn of RD7                                                    
    }                                                       // break the while loop and buggy will full-speed ahead
}