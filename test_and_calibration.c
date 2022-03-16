#include <xc.h>
#include "dc_motor.h"
#include "serial.h"
#include <stdio.h>
#include "movement.h"
#include "test_and_calibration.h"
#include "color.h"

unsigned int setup(struct white_card *white,struct color_rgb*amb,struct DC_motor *mL, struct DC_motor *mR)
{
    unsigned int amb_light = 0;
    LED_OFF();
    LATDbits.LATD7 = 1;
    LATHbits.LATH3 = 1;
    // Press RF2: Calibration with white card
    while (PORTFbits.RF2);
    if (!PORTFbits.RF2){calibrate_white(white); LATDbits.LATD7 = 0; __delay_ms(500);}
    // Press RF3: Measurement of the ambient light
    while (PORTFbits.RF3);
    if (!PORTFbits.RF3){amb_light = amb_light_measure(amb); LATHbits.LATH3 = 0; __delay_ms(500);}
    
    //LATDbits.LATD7 = 1;
    while(1){
        while (PORTFbits.RF3 && PORTFbits.RF2){LATDbits.LATD7 = 1; LATHbits.LATH3 = !LATHbits.LATH3; __delay_ms(200);}
        if (!PORTFbits.RF3){calibration_motor(mL,mR); LATDbits.LATD7 = 0;} // This line is not working as intended
        if (!PORTFbits.RF2){LATDbits.LATD7 = 0; LATHbits.LATH3 = 0; __delay_ms(500); return amb_light;}
    }
}



void color_data_collection(struct color_rgb *m)
{
    int i = 0; int j = 0; int k = 0; int x = 0;
    for (i = 0; i < 1; ++i){
        LED_C();
        __delay_ms(100);
        read_color(m);
        color_display(m);
    }  
    //color_predict(00000);
    for (j = 0; j < 1; ++j){
        LED_R();
        __delay_ms(100);
        read_color(m);
        color_display(m);
    }
    //color_predict(00000);
    for (k = 0; k < 1; ++k){
        LED_G();
        __delay_ms(100);
        read_color(m);
        color_display(m);
    }
    //color_predict(00000);
    for (x = 0; x < 1; ++x){
        LED_B();
        __delay_ms(100);
        read_color(m);
        color_display(m);
    }
    color_predict(1);
    LED_C();
}

void test_action(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL,mR);
    __delay_ms(2000);
    turn45(mL,mR,8,1);              //turn left 45 degrees for 8 times
    stop(mL,mR);
    __delay_ms(2000);
    turn45(mL,mR,8,2);              //turn right 45 degrees for 8 times
    fullSpeedAhead(mL,mR);          //full speed ahead to test whether the buggy go straight properly
    __delay_ms(3000);
    stop(mL,mR);
    turn45(mL,mR,4,1);              //turn left 180 degrees
    fullSpeedAhead(mL,mR);          //full speed ahead to test whether the buggy go straight properly
    __delay_ms(3000);
    stop(mL,mR);
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
        // Test 5: Motor calibration
        if (test_code == 5){
            test_action(mL,mR);
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
//    LATDbits.LATD7 = 1;
//    while (PORTFbits.RF2)
//    if (!PORTFbits.RF2){LATDbits.LATD7 = 0;__delay_ms(500);}
//    LATDbits.LATD7 = 0;
    
}

