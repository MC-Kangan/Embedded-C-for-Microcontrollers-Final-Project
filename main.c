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
#define TEST 0

// Color code: 
// 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
unsigned char color = 0;  

void main(void){
    // Initiate
    I2C_2_Master_Init();
    color_click_init();
    initDCmotorsPWM(199);
    buggylight_init();
    initUSART4();
    Timer0_init();
    Interrupts_init();
    pin_init(); //initiate RGB pins, RF2 and RF3 pins for motor calibrations, BATVERSE pins for voltage measurement 

    struct color_rgb rgb, amb;
    struct white_card white;
    //white.RR = 627; white.RG = 43; white.RB = 88;
    //white.GR = 98; white.GG = 347; white.GB = 126;
    //white.BR = 53; white.BG = 46; white.BB = 97;
    struct DC_motor motorL, motorR;
 
    initDCmotors_parameter(&motorL, &motorR);   
    unsigned char stop_signal = 0;
    unsigned int amb_light = 0;
    //unsigned char setup = 0;
    unsigned char accident = 0; 
    unsigned int start_time= 0;
    unsigned int stop_time = 0;
    
  //    calibration(&motorL, &motorR);  
    if (TEST == 0){
        setup(amb_light,&white,&amb,&motorL,&motorR);
//        while(!setup){
//            LED_OFF();
//            LATDbits.LATD7 = 1;
//            LATHbits.LATH3 = 1;
//            while (PORTFbits.RF2);
//            if (!PORTFbits.RF2){calibrate_white(&white); LATDbits.LATD7 = 0; __delay_ms(500);}
//            while (PORTFbits.RF3);
//            if (!PORTFbits.RF3){amb_light = amb_light_measure(&amb); LATHbits.LATH3 = 0; __delay_ms(500);}
//            LATDbits.LATD7 = 1;
//            if (!PORTFbits.RF3){calibration_motor(&motorL, &motorR);} // This line is not working as intended
//            while (PORTFbits.RF2);
//            if (!PORTFbits.RF2){LATDbits.LATD7 = 0; setup = 1;__delay_ms(500);}
//        }
    }
      
    while(1){
		if (TEST == 1){test_function(3, &rgb, &white, &motorL, &motorR);}
        if (TEST == 0){
            straight_action(&motorL, &motorR, amb_light, start_time, stop_time);
            distance_memory(&motorL, &motorR, start_time, stop_time);
            //read_color(&rgb);
            //color_display(&rgb);
//            T0CON0bits.T0EN=1;	//start the timer (energy saving for the timer to count only when buggy is going straight)
//            start_move = centisecond;
//            while (stop_signal == 0){
//                fullSpeedAhead(&motorL, &motorR);
//                stop_signal = distance_measure(&motorL, &motorR, amb_light);
//            }            //            T0CON0bits.T0EN=0;  //stop the timer (energy saving for the timer to count only when buggy is going straight)
//            stop_move = centisecond;
//            if ((stop_time-start_time)>10){
//                memory[array_index] = (stop_time-start_time-3);
//                array_index++;
//            }
//            else{
//                accident++;
//                color_predict(accident);
//                if (accident >= 5){
//                    goback(&motorL, &motorR);
//                    accident = 0;
//                }
//            }                   
//            stop(&motorL, &motorR);
//            __delay_ms(1000);
            short_burst(&motorL, &motorR);
            color = detect_color(&rgb, &white);
            color = verify_color(color, &rgb, &white);
            if (color!= 0){turning_action(color, &motorL, &motorR); color = 0;}
            stop_signal = 0;
        }
    }
}

