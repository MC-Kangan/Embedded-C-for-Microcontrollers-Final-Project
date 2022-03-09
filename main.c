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
#include <stdio.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 
#define TURNING_POWER_L 40 // This number needs to be adjusted according to different floor condition
#define TURNING_POWER_R  40 // This number needs to be adjusted according to different floor condition
#define TURNING_STOPTIME_L 600 
#define TURNING_STOPTIME_R 700 
#define TURNING_STOPTIME_180 1700 
 #define TEST 1
//#define TEST 0 

// Color code: 
// 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
unsigned char color = 2;   
      
void main(void){
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM(199);
    
    initUSART4();
    
    struct color_rgb rgb;
    struct white_card white;
    //white.RR = 627; white.RG = 43; white.RB = 88;
    //white.GR = 98; white.GG = 347; white.GB = 126;
    //white.BR = 53; white.BG = 46; white.BB = 97;
    struct DC_motor motorL, motorR;

    initDCmotors_parameter(&motorL, &motorR);
    calibration_init();
    
    TRISFbits.TRISF6 = 0; //output on RF6 (BAT-VERSE), detects voltage of motor
    LATFbits.LATF6 = 0; //output on BAT-VERSE set on (power) 
    
    TRISGbits.TRISG1 = 0; //output on RG1 (LED_R)
    TRISFbits.TRISF7 = 0; //output on RF7 (LED_G)
    TRISAbits.TRISA4 = 0; //output on RA4 (LED_B)
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_G set on (power)
    LATAbits.LATA4 = 1; // output LED_B set on (power)
        
    unsigned char color = 0;  
    unsigned char complete = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int x = 0;
    
    __delay_ms(3000);
    calibrate_white(&white);
    __delay_ms(3000);
    
    //calibration(&motorL, &motorR);
    while(1){
        //test_movement(&motorL, &motorR);
		if (TEST == 1){
            color = detect_color(&rgb, &white);
            //LED_R();
            read_color(&rgb);
            //check_color_reading(&rgb, &white);
            color_predict(color);
            __delay_ms(200);
            }
        
        if (TEST == 2){
            while (complete == 0){
                for (i = 0; i < 10; ++i){
                    LED_C();
                    read_color(&rgb);
                    color_display(&rgb);
                   __delay_ms(100);
                }  
                color_predict(00000);
                for (j = 0; j < 10; ++j){
                    LED_R();
                    read_color(&rgb);
                    color_display(&rgb);
                    __delay_ms(100);
                }
                color_predict(00000);
                for (k = 0; k < 10; ++k){
                    LED_G();
                    read_color(&rgb);
                    color_display(&rgb);
                    __delay_ms(100);
                }
                color_predict(00000);
                for (x = 0; x < 10; ++x){
                    LED_B();
                    read_color(&rgb);
                    color_display(&rgb);
                    __delay_ms(100);
                }
                color_predict(00000);
                complete = 1;
                LED_C();
            }
        }
//
//        if (TEST == 0){
            //detect_color_C(&rgb);
            //while (color == 0){fullSpeedAhead(&motorL, &motorR);}
            //stop(&motorL, &motorR);
            //__delay_ms(1000);
            //movement(color,&motorL,&motorR);
            //color = 0;
            //color_predict(00000);
            //__delay_ms(500);
            //check_color(color, &rgb);
//        }
    }
}

