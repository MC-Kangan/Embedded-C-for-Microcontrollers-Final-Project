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
#define TEST 0              // Normal mode(Test=0), Test mode(Test=1)

// Color code: 
// 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
unsigned char color = 0;  //global variable

void main(void){
    
    // Structure used in the program
    struct color_rgb rgb, amb;
    struct white_card white;
    struct DC_motor motorL, motorR;
    
    // Initiation
    color_click_init();     // color click initiation including I2C initiation(color.c)
    pin_init();             // RGB initiation, RF2, RF3 button initiation, clicker board light and buggy light initiation(color.c)
    initDCmotorsPWM(199);   // TRIS and LAT registers for PWM initiation(motor.c)
    initDCmotors_parameter(&motorL, &motorR);  // motor parameter initiation(motor.c)
    initUSART4();           // initiation of USART4 for reception and transmission (serial.c)
    Timer0_init();          // Timer Initiation(timers.c)
    Interrupts_init();      // Interrupt Initiation(interrupt.c)
    
    // Defined local variables (will be discussed in the following codes and README file)
    unsigned char stop_signal = 0;
    unsigned int amb_light = 0;
    unsigned char accident = 0; 
    unsigned int start_time= 0;
    unsigned int stop_time = 0;

    if (TEST == 0){     //Normal mode
        amb_light = setup(&white, &amb, &motorL, &motorR);  // setup function, including calibration white card and ambient light calibration, more detail in test_and_calibration.c
    }
      
    while(1){
		if (TEST == 1){test_function(5, &rgb, &white, &motorL, &motorR);}  // Test mode, call test function
        if (TEST == 0){                                                    // Normal mode
            T0CON0bits.T0EN=1;                          //start the timer (energy saving for the timer to count only when buggy is going straight)
            start_time = centisecond;                   //record the start time in centisecond
            while (stop_signal == 0){                   //while the buggy doesn't detect the wall or color card, stop signal == 0
                fullSpeedAhead(&motorL, &motorR);       //buggy will keep moving forward
                stop_signal = detect_wall(&motorL, &motorR, amb_light);     //if the buggy detects the wall or color card, stop signal == 1 and the buggy will stop moving(Line 77 & 78)
            }
            // before stop the buggy, record the forward moving duration into memory first     
            T0CON0bits.T0EN=0;                          //stop the timer (energy saving for the timer to count only when buggy is going straight)
            stop_time = centisecond;                    //record the stop time in centisecond
            if ((stop_time-start_time)>10){             //only record the forward moving duration when it is greater than 10 centiseconds (1 second))
                memory[array_index] = (stop_time-start_time-3);             //record the forward moving duration in memory
                array_index++;                                              // increase array index position by 1 for next recording
            }
            else{                                       //if the forward moving duration is less than 1 second
                accident++;                             //it means the buggy runs into an accident
                if (accident >= 10){                    //if the accidents happen more than 10 times, the buggy is identified as getting lost
                    goback(&motorL, &motorR);           //ask the buggy to go back to the starting point
                    accident = 0;                       //clear the number of accidents
                }
            }                   
            stop(&motorL, &motorR);                     //stop the buggy for 1.5 seconds. 500ms builtin and extra 1000ms
            __delay_ms(1000);
            short_burst(&motorL, &motorR);              //the buggy will short burst to the wall and detect the color
            color = detect_color(&rgb, &white);         //detect the color
            color = verify_color(color, &rgb, &white);  //verify the color
            if (color!= 0){turning_action(color, &motorL, &motorR); color = 0;}     //if color is not 0 (black), call the turning_action function
            stop_signal = 0;                            //reset the stop signal as 0 and restart the while loop
        }
    }
}