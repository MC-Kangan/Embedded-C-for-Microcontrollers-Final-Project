/*
 * File:   test_and_calibration.c
 * Author: Kangan Chen
 *
 * Created on March 12, 2022, 5:35 PM
 */
#include <xc.h>
#include "dc_motor.h"
#include "serial.h"
#include <stdio.h>
#include "movement.h"
#include "test_and_calibration.h"
#include "color.h"


void setup_init(void){
    // setup pin for output (connected to LED1 - RD7)
    LATDbits.LATD7=0;   //set initial output state - LATx registers (output latch); light on (1); light off (0)
    TRISDbits.TRISD7=0; //set TRIS value for pin (output) - TRISx registers (data direction); Tris determines either output(0) of input(1)
    
     // setup pin for output (connected to LED2 - RH3)
    LATHbits.LATH3=0;   //set initial output state - LATx registers (output latch)
    TRISHbits.TRISH3=0; //set TRIS value for pin (output) - TRISx registers (data direction)
    
    // setup pin for input (connected to button1 - RF2)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin  
    
    // setup pin for input (connected to button2 - RF3)
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
}

void test_function(unsigned char test_code, struct color_rgb *m, struct white_card *w, struct DC_motor *mL, struct DC_motor *mR)
{
    unsigned char complete = 0;
    unsigned char color = 0;
    unsigned char stop_signal = 0;
    unsigned int amb_light = 0;
    
    if (test_code == 2){calibrate_white(w);}
    if (test_code == 4){amb_light = amb_light_measure(m);}
    
    while(1){
        // Test 1: Read RGBC data in white light
        if (test_code == 1){
            LED_C(); // Turn on White light
            read_color(m); 
            color_display(m);
        }
        // Test 2: Detect color in RGB light and predict it via serial port
        if (test_code == 2){
            color = detect_color(m,w);
            //color_display(m);
            color_predict(color);
        }
        // Test 3: Collect data for different color cards
        if (test_code == 3){
            while (complete == 0){
                color_data_collection(m);
                complete = 1;
            }
        }   
        // Test 4: Distance measure
        if (test_code == 4){
            while (stop_signal == 0){
                fullSpeedAhead(mL, mR);
                stop_signal = distance_measure(mL, mR, amb_light);
            }
            stop(mL, mR);
            __delay_ms(1000);
            stop_signal = 0;
        }
    }
}

void calibration_motor(struct DC_motor *mL, struct DC_motor *mR)
{ 
    while (1) {  // press both to exit and start buggy
        LATDbits.LATD7 = 1;
        LATHbits.LATH3 = 1;
        
        if (!PORTFbits.RF2) {
            __delay_ms(300);
            if (!PORTFbits.RF2){LATDbits.LATD7 = 0; __delay_ms(300); SENSITIVITY += 5; }
            if (!PORTFbits.RF3){break;}    // end the infinite while loop if RF3 is also pressed
        }
        if (!PORTFbits.RF3) {
            __delay_ms(300);
            if (!PORTFbits.RF2){break;}
            if (!PORTFbits.RF3){LATHbits.LATH3 = 0; __delay_ms(300); SENSITIVITY -= 5; }    // end the infinite while loop if RF3 is also pressed
            }
    }
    LATDbits.LATD7 = 0;
    LATHbits.LATH3 = 0;
    test_action(mL, mR);
}

