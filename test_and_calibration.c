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
    turnLeft(mL,mR,180);
    fullSpeedAhead_test(mL,mR);
    turnLeft(mL,mR,135);
    fullSpeedAhead_test(mL,mR);
    turnRight(mL,mR,135);
    fullSpeedAhead_test(mL,mR);
    turnLeft(mL,mR,135);
    fullSpeedAhead_test(mL,mR);
    turnRight(mL,mR,135);
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

