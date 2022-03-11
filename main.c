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
#include <stdio.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 
 #define TEST 4
//#define TEST 0 
// Color code: 
// 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
unsigned char color = 0;  

void main(void){
    Interrupts_init();
    Timer0_init();
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM(199);
    buggylight_init();
    initUSART4();
    
    struct color_rgb rgb, amb;
    struct white_card white;
    //white.RR = 627; white.RG = 43; white.RB = 88;
    //white.GR = 98; white.GG = 347; white.GB = 126;
    //white.BR = 53; white.BG = 46; white.BB = 97;
    struct DC_motor motorL, motorR;

    initDCmotors_parameter(&motorL, &motorR);
    pin_init(); //initiate RGB pins, RF2 and RF3 pins for motor calibrations, BATVERSE pins for voltage measurement 
    
    
    if (TEST == 1 || TEST == 3 || TEST == 4){
        LED_OFF();
        toggle_light(1,2);
        __delay_ms(1000);
        calibrate_white(&white);
        __delay_ms(1000);
        LED_OFF();
        toggle_light(1,2);
    }
    
    unsigned char complete = 0;
    unsigned char stop_signal = 0;
//    calibration(&motorL, &motorR);
    
    unsigned int amb_light = 0;
    amb_light = amb_light_measure(&amb);
        
    
    while(1){
        //action(color, &motorL, &motorR);
        //test_action(&motorL, &motorR);
		if (TEST == 1){
            LED_C();
            read_color(&rgb);
            //color_display(&rgb);
            
            //color = detect_color(&rgb, &white);
            //color_predict(color);
            }
        
        if (TEST == 2){
            while (complete == 0){
                color_data_collection(&rgb);
                complete = 1;
            }
        }
        
        if (TEST == 3){
            while (color == 0){
                short_burst(&motorL, &motorR);
                color = detect_color(&rgb, &white);
            }
            verify_color(color, &rgb, &white);
            action(color, &motorL, &motorR);
            color = 0;
        }
        if (TEST == 4){
            
            //read_color(&rgb);
            //color_display(&rgb);
            while (stop_signal == 0){
                fullSpeedAhead(&motorL, &motorR);
                stop_signal = distance_measure(&motorL, &motorR, amb_light);
            }
            
            stop(&motorL, &motorR);
            __delay_ms(1000);
            color = detect_color(&rgb, &white);
            color = verify_color(color, &rgb, &white);
            action(color, &motorL, &motorR);
            color = 0;
            stop_signal = 0;
            stop_signal = 0;
        }
    }
}

