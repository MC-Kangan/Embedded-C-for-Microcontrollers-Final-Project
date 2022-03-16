 #include <xc.h>
#include <stdio.h>
#include "color.h"
#include "dc_motor.h"
#include "serial.h"
#include "movement.h"
#include "timers.h"
#include "test_and_calibration.h"

void pin_init(void)
{   TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin     
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
    
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

void setup(unsigned int amb_light, struct white_card *white,struct color_rgb*amb,struct DC_motor *mL, struct DC_motor *mR)
{
    LED_OFF();
    LATDbits.LATD7 = 1;
    LATHbits.LATH3 = 1;
    while (PORTFbits.RF2);
    if (!PORTFbits.RF2){calibrate_white(white); LATDbits.LATD7 = 0; __delay_ms(500);}
    while (PORTFbits.RF3);
    if (!PORTFbits.RF3){amb_light = amb_light_measure(amb); LATHbits.LATH3 = 0; __delay_ms(500);}
    LATDbits.LATD7 = 1;
    if (!PORTFbits.RF3){calibration_motor(mL,mR);} // This line is not working as intended
    while (PORTFbits.RF2);
    if (!PORTFbits.RF2){LATDbits.LATD7 = 0;__delay_ms(500);}
}

void short_burst(struct DC_motor *mL, struct DC_motor *mR)
{
    fullSpeedAhead(mL,mR);
    __delay_ms(500);
    stop(mL,mR);
}

void straight_action(struct DC_motor *mL, struct DC_motor *mR, unsigned int amb_light, unsigned int start_time, unsigned int stop_time)
{  
    unsigned char stop_signal = 0;
    //read_color(&rgb);
    //color_display(&rgb);
    T0CON0bits.T0EN=1;	//start the timer (energy saving for the timer to count only when buggy is going straight)
    start_time = centisecond;
    while (stop_signal == 0){
        fullSpeedAhead(mL,mR);
        stop_signal = distance_measure(mL,mR,amb_light);
    }            
    T0CON0bits.T0EN=0;  //stop the timer (energy saving for the timer to count only when buggy is going straight)
    stop_time = centisecond;    
    stop(mL,mR);
    __delay_ms(1000);
}

void distance_memory(struct DC_motor *mL, struct DC_motor *mR, unsigned int start_time, unsigned int stop_time)
{   
    unsigned char accident = 0; 
    if ((stop_time-start_time)>10){
        memory[array_index] = (stop_time-start_time-3);
        array_index++;
    }
    else{
        accident++;
        color_predict(accident);
        if (accident >= 5){
            goback(mL,mR);
            accident = 0;
        }
    }                   
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
    turn45(mL,mR,4,1);
    fullSpeedBack(mL,mR,2);
    array_index--;
    while(array_index >= 0){
        fullSpeedAhead(mL,mR);
        for (unsigned int i=0; i<memory[array_index]; i++) {__delay_ms(100);}
        stop(mL,mR);
        if (array_index == 0){break;}   
        array_index--;
        if (memory[array_index] == 1){turn45(mL,mR,2,1);fullSpeedBack(mL,mR,2);array_index--;}
        else if (memory[array_index] == 2){turn45(mL,mR,2,2);fullSpeedBack(mL,mR,2);array_index--;}
        else if (memory[array_index] == 3){turn45(mL,mR,4,1);fullSpeedBack(mL,mR,2);array_index--;}
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