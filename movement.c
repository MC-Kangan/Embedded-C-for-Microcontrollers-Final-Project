 #include <xc.h>
#include <stdio.h>
#include "color.h"
#include "dc_motor.h"
#include "serial.h"
#include "movement.h"
#include "timers.h"


void short_burst(struct DC_motor *mL, struct DC_motor *mR)
{
    fullSpeedAhead(mL,mR);
    __delay_ms(500);
    stop(mL,mR);
}

void action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR)
{    if (color != 0){
        if (color == 1){            //Red
            short_reverse(mL,mR,1);   //Reverse moving for a small distance to leave space for turning
            turn45(mL, mR, 2, 2);
            //turnRight(mL,mR,90);    //Turn Right 90 degrees
            memory[array_index] = 1;
            array_index++;
        }
        if (color == 2){            //green
            short_reverse(mL,mR,1);   //Reverse moving for a small distance to leave space for turning
            turn45(mL, mR, 2, 1);
            //turnLeft(mL,mR,90);     //Turn Left 90 degrees
            memory[array_index] = 2;
            array_index++;
        }
        if (color == 3){            //blue
            short_reverse(mL,mR,1);   //Reverse moving for a small distance to leave space for turning
            turn45(mL, mR, 4, 1);
            //turnLeft(mL,mR,180);    //Turn 180 degrees
            memory[array_index] = 3;
            array_index++;
        }
        if (color == 4){            //yellow
            short_reverse(mL,mR,3);  //Reverse a square distance
            turn45(mL, mR, 2, 2);
            //turnRight(mL,mR,90);    //Turn Right 90 degrees        
            memory[array_index] = 4;
            array_index++;
        }
        if (color == 5){            //pink
            short_reverse(mL,mR,3);  //Reverse a square distance
            turn45(mL, mR, 2, 1);
            //turnLeft(mL,mR,90);     //Turn Left 90 degrees
            memory[array_index] = 5;
            array_index++;
        }
        if (color == 6){            //orange
            short_reverse(mL,mR,1);   //Reverse moving for a small distance to leave space for turning
            turn45(mL, mR, 3, 2);
            //turnRight(mL,mR,135);   //Turn Right 135 degrees
            memory[array_index] = 6;
            array_index++;
        }
        if (color == 7){            //light blue
            short_reverse(mL,mR,1);   //Reverse moving for a small distance to leave space for turning
            turn45(mL, mR, 3, 1);
            //turnLeft(mL,mR,135);    //Turn Left 135 degrees
            memory[array_index] = 7;
            array_index++;
        }
        if (color == 8){            //white
            toggle_light(2,1);
            short_reverse(mL,mR,1);   //Reverse moving for a small distance to leave space for turning
            goback(mL,mR);          //Go back to origin
        }
    }
}

void pin_init(void)
{   TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin     
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
    
//    TRISFbits.TRISF6 = 0; //output on RF6 (BAT-VERSE), detects voltage of motor
//    LATFbits.LATF6 = 0; //output on BAT-VERSE set on (power) 
    
    TRISGbits.TRISG1 = 0; //output on RG1 (LED_R)
    TRISFbits.TRISF7 = 0; //output on RF7 (LED_G)
    TRISAbits.TRISA4 = 0; //output on RA4 (LED_B)
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_G set on (power)
    LATAbits.LATA4 = 1; // output LED_B set on (power)
    
    // pins connected to buttons
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7 = 0;
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
}

void goback(struct DC_motor *mL, struct DC_motor *mR)
{   turn45(mL, mR, 4, 1);
    //turnRight(mL,mR,180);
    short_reverse(mL,mR,2);
    array_index--; 
    while(array_index >= 0){
        fullSpeedAhead(mL,mR);
        for (unsigned int i=0; i<memory[array_index]; i++) {__delay_ms(100);}
        stop(mL,mR);
        if (array_index == 0){break;}   
        array_index--;
        if (memory[array_index] == 1){turn45(mL, mR, 2, 1);short_reverse(mL,mR,2);array_index--;}
        else if (memory[array_index] == 2){turn45(mL, mR, 2, 2);short_reverse(mL,mR,2);array_index--;}
        else if (memory[array_index] == 3){turn45(mL, mR, 4, 1);short_reverse(mL,mR,2);array_index--;}
        else if (memory[array_index] == 4){turn45(mL, mR, 2, 1);short_reverse(mL,mR,3);array_index--;}
        else if (memory[array_index] == 5){turn45(mL, mR, 2, 2);short_reverse(mL,mR,3);array_index--;}
        else if (memory[array_index] == 6){turn45(mL, mR, 3, 1);short_reverse(mL,mR,2);array_index--;}
        else if (memory[array_index] == 7){turn45(mL, mR, 3, 2);short_reverse(mL,mR,2);array_index--;}
    }
    LATDbits.LATD7 = 1;     // turn on RD7 to indicate users to press RF2
    while(PORTFbits.RF2){   // wait RF2 to be pressed, otherwise the buggy will stop forever
        stop(mL,mR); 
        if (!PORTFbits.RF2){LATDbits.LATD7 = 0;break;}      // if RF2 is pressed, turn of RD7
                                                            // break the while loop and buggy will fullspeed ahead
    }
}