 // CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "color.h"
#include "i2c.h"
#include "dc_motor.h"
#include "movement.h"
#include "timers.h"
#include "interrupts.h"
#include "serial.h"
#include "test_and_calibration.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 
#define TEST 0

// Color code: 
// 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
unsigned char color = 0;  

void main(void){
    
    struct color_rgb rgb, amb;
    struct white_card white;
    struct DC_motor motorL, motorR;
    
    // Initiation
    // I2C_2_Master_Init();
    color_click_init();     // color click initiation including I2C initiation(color.c)
    pin_init();             // RGB initiation, RF2, RF3 button initiation, clicker board light and buggy light initiation(color.c)
    initDCmotorsPWM(199);   // TRIS and LAT registers for PWM initiation(motor.c)
    initDCmotors_parameter(&motorL, &motorR);  // motor parameter initiation(motor.c)
    initUSART4();           // initiation of USART4 for reception and transmission (serial.c)
    Timer0_init();          // Timer Initiation(timers.c)
    Interrupts_init();      // Interrupt Initiation(interrupt.c)
    
    unsigned char stop_signal = 0;
    unsigned int amb_light = 0;
    unsigned char accident = 0; 
    unsigned int start_time= 0;
    unsigned int stop_time = 0;

    if (TEST == 0){
        amb_light = setup(&white, &amb, &motorL, &motorR);
    }
      
    while(1){
		if (TEST == 1){test_function(2, &rgb, &white, &motorL, &motorR);}
        if (TEST == 0){ 
            T0CON0bits.T0EN=1;	//start the timer (energy saving for the timer to count only when buggy is going straight)
            start_time = centisecond;
            while (stop_signal == 0){
                fullSpeedAhead(&motorL, &motorR);
                stop_signal = detect_wall(&motorL, &motorR, amb_light);
            }            
            T0CON0bits.T0EN=0;  //stop the timer (energy saving for the timer to count only when buggy is going straight)
            stop_time = centisecond;
            if ((stop_time-start_time)>10){
                memory[array_index] = (stop_time-start_time-3);
                array_index++;
            }
            else{
                accident++;
                color_predict(accident);
                if (accident >= 5){
                    goback(&motorL, &motorR);
                    accident = 0;
                }
            }                   
            stop(&motorL, &motorR);
            __delay_ms(1000);
            short_burst(&motorL, &motorR);
            color = detect_color(&rgb, &white);
            color = verify_color(color, &rgb, &white);
            if (color!= 0){turning_action(color, &motorL, &motorR); color = 0;}
            stop_signal = 0;
        }
    }
}

