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
    unsigned char setup = 0;
    unsigned char accident = 0;
    if (TEST == 0){
        while(!setup){
            LED_OFF();
            LATDbits.LATD7 = 1;
            LATHbits.LATH3 = 1;
            while (PORTFbits.RF2);
            if (!PORTFbits.RF2){calibrate_white(&white); LATDbits.LATD7 = 0; __delay_ms(500);}
            while (PORTFbits.RF3);
            if (!PORTFbits.RF3){amb_light = amb_light_measure(&amb); LATHbits.LATH3 = 0; __delay_ms(500);}
            LATDbits.LATD7 = 1;
            if (!PORTFbits.RF3){calibration_motor(&motorL, &motorR);} // This line is not working as intended
            while (PORTFbits.RF2);
            if (!PORTFbits.RF2){LATDbits.LATD7 = 0; setup = 1;__delay_ms(500);}
        }
    }
    
   
    while(1){

		if (TEST == 1){
            //test_function(3, &rgb, &white, &motorL, &motorR);
            unsigned char i = 0, j = 0; 
//            for (i = 0; i < 10; i++){
//                turn45(&motorL, &motorR,1,1);
//            }
//            
//            for (j = 0; j < 10; j++){
//                turn45(&motorL, &motorR,1,2);
//            }
            fullSpeedAhead(&motorL, &motorR);
            //__delay_ms(3000);
            
        }
        if (TEST == 0){
            //read_color(&rgb);
            //color_display(&rgb);
            T0CON0bits.T0EN=1;	//start the timer
            start_move = centisecond;
            while (stop_signal == 0){
                fullSpeedAhead(&motorL, &motorR);
                stop_signal = distance_measure(&motorL, &motorR, amb_light);
            }            
            T0CON0bits.T0EN=0;  //stop the timer
            stop_move = centisecond;
            if ((stop_move-start_move)>1){
                memory[array_index] = (stop_move-start_move);
                array_index++;
            }
            else{
                accident++;
                if (accident >= 10){
                    goback(&motorL, &motorR);
                    accident = 0;
                }
            }
            
//            char buf[100];
//            sprintf(buf,"%d\r\n",(stop_move-start_move));
//            sendStringSerial4(buf);
            
            stop(&motorL, &motorR);
            __delay_ms(1000);
            short_burst(&motorL, &motorR);
            color = detect_color(&rgb, &white);
            color = verify_color(color, &rgb, &white);
            if (color!= 0){action(color, &motorL, &motorR); color = 0;}
            stop_signal = 0;
        }
    }
}

