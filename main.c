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
//#include "movement.h"
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
unsigned char color = 1;   
      
void main(void){
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM(199);
    
    initUSART4();
    
    struct color_rgb rgb;
    struct DC_motor motorL, motorR;

    initDCmotors_parameter(&motorL, &motorR);
    
    TRISFbits.TRISF6 = 0; //output on RF6 (BAT-VERSE), detects voltage of motor
    LATFbits.LATF6 = 0; //output on BAT-VERSE set on (power) 
    
    TRISGbits.TRISG1 = 0; //output on RG1 (LED_R)
    TRISFbits.TRISF7 = 0; //output on RF7 (LED_G)
    TRISAbits.TRISA4 = 0; //output on RA4 (LED_B)
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_G set on (power)
    LATAbits.LATA4 = 1; // output LED_B set on (power)
        
    unsigned char color = 0;
   
    while(1){
		if (TEST){
        LED_G(&rgb);
        color_display(&rgb);
        __delay_ms(200);
        }
        if (!TEST){
            detect_color_C(&rgb);
            while (color == 0){fullSpeedAhead(&motorL, &motorR);}
            stop(&motorL, &motorR);
            __delay_ms(1000);
            movement(color,&motorL,&motorR);
            color = 0;

            //__delay_ms(500);
            //check_color(color, &rgb);
        }
    }
}

