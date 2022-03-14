// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "serial.h"
#include "color.h"
#include "i2c.h"
#include "movement.h"
#include "interrupts.h"
#include "timers.h"
#include "test_and_calibration.h"
#include <stdio.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 
#define TEST 1

// Color code: 
// 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
unsigned char color = 0;  
unsigned int start_move;
unsigned int stop_move;

void main(void){
    
    
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM(199);
    buggylight_init();
    initUSART4();
    Timer0_init();
    Interrupts_init();
    
    struct color_rgb rgb, amb;
    struct white_card white;
    //white.RR = 627; white.RG = 43; white.RB = 88;
    //white.GR = 98; white.GG = 347; white.GB = 126;
    //white.BR = 53; white.BG = 46; white.BB = 97;
    struct DC_motor motorL, motorR;
    pin_init(); //initiate RGB pins, RF2 and RF3 pins for motor calibrations, BATVERSE pins for voltage measurement 
    initDCmotors_parameter(&motorL, &motorR);
    
    unsigned char stop_signal = 0;
//    calibration(&motorL, &motorR);
    unsigned int amb_light = 0;
//    test_action(&motorL, &motorR);  
    if (TEST == 0){
        calibrate_white(&white);
        amb_light = amb_light_measure(&amb);
    }
    
    while(1){

		if (TEST == 1){
            test_function(3, &rgb, &white, &motorL, &motorR);
        }
        if (TEST == 0){
            //read_color(&rgb);
            //color_display(&rgb);
            T0CON0bits.T0EN=1;	//start the timer
            start_move = second;
            while (stop_signal == 0){
                fullSpeedAhead(&motorL, &motorR);
                stop_signal = distance_measure(&motorL, &motorR, amb_light);
            }            
            T0CON0bits.T0EN=0;  //stop the timer
            stop_move = second;
            if ((stop_move-start_move)>1){
                memory[array_index] = (stop_move-start_move);
                color_predict(array_index);
                color_predict(memory[array_index]);
                color_predict(200);
                array_index++;
            }
            stop(&motorL, &motorR);
            __delay_ms(1000);
            short_burst_back(&motorL, &motorR);
            color = detect_color(&rgb, &white);
            color = verify_color(color, &rgb, &white);
            //if (stop_move-start_move)
            
            action(color, &motorL, &motorR);
            color = 0;
            stop_signal = 0;
        }
    }
}

