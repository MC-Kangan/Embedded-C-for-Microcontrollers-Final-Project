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
    __delay_ms(300);
    stop(mL,mR);
    __delay_ms(500);
}

void action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR)
{   T0CON0bits.T0EN=1;	//start the timer
    start_move = second;
    fullSpeedAhead(mL,mR);
    if (color != 0){
        T0CON0bits.T0EN=0;//stop the timer
        stop_move = second;
        memory[array_index] = (start_move-stop_move);
        array_index++;
        stop(mL,mR);
        if (color == 1){            //Red
            short_reverse(mL,mR);   //Reverse moving for a small distance to leave space for turning
            turnRight(mL,mR,90);    //Turn Right 90 degrees
            memory[array_index] = 1;
            array_index++;
        }
        if (color == 2){            //green
            short_reverse(mL,mR);   //Reverse moving for a small distance to leave space for turning
            turnLeft(mL,mR,90);     //Turn Left 90 degrees
            memory[array_index] = 2;
            array_index++;
        }
        if (color == 3){            //blue
            short_reverse(mL,mR);   //Reverse moving for a small distance to leave space for turning
            turnLeft(mL,mR,180);    //Turn 180 degrees
            memory[array_index] = 3;
            array_index++;
        }
        if (color == 4){            //yellow
            reverse_square(mL,mR);  //Reverse a square distance
            turnRight(mL,mR,90);    //Turn Right 90 degrees        
            memory[array_index] = 4;
            array_index++;
        }
        if (color == 5){            //pink
            reverse_square(mL,mR);  //Reverse a square distance
            turnLeft(mL,mR,90);     //Turn Left 90 degrees
            memory[array_index] = 5;
            array_index++;
        }
        if (color == 6){            //orange
            short_reverse(mL,mR);   //Reverse moving for a small distance to leave space for turning
            turnRight(mL,mR,135);   //Turn Right 135 degrees
            memory[array_index] = 6;
            array_index++;
        }
        if (color == 7){            //light blue
            short_reverse(mL,mR);   //Reverse moving for a small distance to leave space for turning
            turnLeft(mL,mR,135);    //Turn Left 135 degrees
            memory[array_index] = 7;
            array_index++;
        }
        if (color == 8){            //white
            short_reverse(mL,mR);   //Reverse moving for a small distance to leave space for turning
            goback(mL,mR);          //Go back to origin
        }
    }
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
{   turnLeft(mL,mR,180);
    while(array_index > 0){ 
        fullSpeedAhead(mL,mR);
        for (unsigned int i=0; i<memory[array_index]; i++) {__delay_ms(1000);}
        stop(mL,mR);
        if (array_index == 0){break;}     
        array_index--;
        
        if (memory[array_index] == 1){turnLeft(mL,mR,90);array_index--;}
        if (memory[array_index] == 2){turnRight(mL,mR,90);array_index--;}
        if (memory[array_index] == 3){turnLeft(mL,mR,180);array_index--;}
        if (memory[array_index] == 4){reverse_square(mL,mR);turnLeft(mL,mR,90);array_index--;}
        if (memory[array_index] == 5){reverse_square(mL,mR);turnRight(mL,mR,90);array_index--;}
        if (memory[array_index] == 6){turnLeft(mL,mR,135);array_index--;}
        if (memory[array_index] == 7){turnRight(mL, mR, -135);array_index--;}
    }
}