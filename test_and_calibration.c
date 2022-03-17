#include <xc.h>
#include "color.h"
#include "dc_motor.h"
#include "test_and_calibration.h"

unsigned int setup(struct white_card *white,struct color_rgb*amb,struct DC_motor *mL, struct DC_motor *mR)
{
    unsigned int amb_light = 0;
    LED_OFF();
    LATDbits.LATD7 = 1;     //Turn on RD7, indicate users to press RF2
    LATHbits.LATH3 = 1;     //Turn on RH3, indicate users to press RF3
    // Press RF2: Calibration with white card
    while (PORTFbits.RF2);
    if (!PORTFbits.RF2){calibrate_white(white); LATDbits.LATD7 = 0; __delay_ms(500);}               // After calibration with white card, turn off RD7
    // Press RF3: Measurement of the ambient light
    while (PORTFbits.RF3);
    if (!PORTFbits.RF3){amb_light = amb_light_measure(amb); LATHbits.LATH3 = 0; __delay_ms(500);}   // After embient light measurement, turn off RH3
    while(1){                                                                                           // Turn on RD7 and toggling RH3 to indicate users press either RF2 or RF3.
        while (PORTFbits.RF3 && PORTFbits.RF2){LATDbits.LATD7 = 1; LATHbits.LATH3 = !LATHbits.LATH3; __delay_ms(200);}
        if (!PORTFbits.RF3){calibration_motor(mL,mR); LATDbits.LATD7 = 0;}                              // if RF3 is pressed, start motor calibration process
        if (!PORTFbits.RF2){LATDbits.LATD7 = 0; LATHbits.LATH3 = 0; __delay_ms(500); return amb_light;} // if RF2 is pressed, start the maze
    }
}




void test_action(struct DC_motor *mL, struct DC_motor *mR)
{
    stop(mL,mR);
    __delay_ms(2000);               //stop for 2.5 seconds(500ms builtin, 2000ms delay) for the ease of placing buggy into correct position
    turn45(mL,mR,8,1);              //turn left 45 degrees for 8 times
    stop(mL,mR);
    __delay_ms(2000);               //stop for 2.5 seconds(500ms builtin, 2000ms delay) for the ease of placing buggy into correct position
    turn45(mL,mR,8,2);              //turn right 45 degrees for 8 times
    fullSpeedAhead(mL,mR);          //full speed ahead for 3 seconds to test whether the buggy go straight properly
    __delay_ms(3000);
    stop(mL,mR);
    turn45(mL,mR,4,1);              //turn left 180 degrees
    fullSpeedAhead(mL,mR);          //full speed ahead for 3 seconds to test whether the buggy go straight properly
    __delay_ms(3000);
    stop(mL,mR);                    // stop
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
                stop_signal = detect_wall(mL, mR, amb_light);
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
    while (1) {                 // turn on RD7 and RH3 on clicker board to indicate the motor calibration function is on
        LATDbits.LATD7 = 1;
        LATHbits.LATH3 = 1;       
        if (!PORTFbits.RF2) {   // if the user presses RF2
            __delay_ms(300);
            if (!PORTFbits.RF2){LATDbits.LATD7 = 0; __delay_ms(300); SENSITIVITY += 5; }    // add sensitivity by 5 for each 300ms if the user is holding RF2 button. The light will toggle in this case to show the user how many times the sensitivity if added
            if (!PORTFbits.RF3){break;}                                                     // end the infinite while loop if RF3 is also pressed(both button are pressed)
        }
        if (!PORTFbits.RF3) {   // if the user presses RF3
            __delay_ms(300);
            if (!PORTFbits.RF2){break;}                                                     // end the infinite while loop if RF2 is also pressed(both button are pressed)
            if (!PORTFbits.RF3){LATHbits.LATH3 = 0; __delay_ms(300); SENSITIVITY -= 5; }    // reduce sensitivity by 5 for each 300ms if the user is holding RF2 button. The light will toggle in this case to show the user how many times the sensitivity if added
            }
    }
    LATDbits.LATD7 = 0;
    LATHbits.LATH3 = 0;
    test_action(mL, mR);      // turn off RD7 and RH3 on clicker board, and start test action function to see whether the motor moves properly.
}

