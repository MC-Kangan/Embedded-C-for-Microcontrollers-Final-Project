#include <xc.h>
#include "dc_motor.h"
#include "serial.h"
#include "color.h"
#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fenv.h>
#include "movement.h"
#include "test_and_calibration.h"

// Function used to initialise pins
void pin_init(void)
{   // Button pins initiation
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin     
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin  
    
    // Clicker2 board LED initiation
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7 = 0;
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
    
    // Color click LED initiation
    TRISGbits.TRISG1 = 0; //output on RG1 (LED_R)
    TRISFbits.TRISF7 = 0; //output on RF7 (LED_G)
    TRISAbits.TRISA4 = 0; //output on RA4 (LED_B)
    
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_G set on (power)
    LATAbits.LATA4 = 1; // output LED_B set on (power)
    
    // Buggy light initiation
    TRISHbits.TRISH1=0; // Head lamps
    TRISDbits.TRISD4=0; // Brake
    TRISDbits.TRISD3=0; // Main beam 
    TRISFbits.TRISF0=0; // Turn L
    TRISHbits.TRISH0=0; // Turn R  
    
    LATHbits.LATH1 = 0; // front white and rear red (reduced)
    LATDbits.LATD4 = 0; // rear red (full)
    LATDbits.LATD3 = 0; // front white (full)
    LATFbits.LATF0 = 0; // TURN-L light
    LATHbits.LATH0 = 0; // TURN-R light
    
}

// Function used to toggle the buggy light
void toggle_light(unsigned char lightnumber, unsigned char toggletime) 
{
    unsigned char i = 0;
    
    for (i = 0; i < toggletime; i++){           // Use for loop to repeat the toggle i times
        if (lightnumber == 1){                  // Buggy light 1 = front white and rear red (reduced)
            LATHbits.LATH1 = !LATHbits.LATH1;   // Toggle light
            __delay_ms(500);
            LATHbits.LATH1 = !LATHbits.LATH1;
            __delay_ms(500);
        }
        if (lightnumber == 2){                  // Buggy light 2 = rear red (full)
            LATDbits.LATD4 = !LATDbits.LATD4; 
            __delay_ms(500);
            LATDbits.LATD4 = !LATDbits.LATD4;
            __delay_ms(500);
        }
        if (lightnumber == 3){                  // Buggy light 3 = front white (full)
            LATDbits.LATD3 = !LATDbits.LATD3; 
            __delay_ms(500);
            LATDbits.LATD3 = !LATDbits.LATD3;
            __delay_ms(500);
        }        
        if (lightnumber == 4){                  // Buggy light 4 = TURN-L light
            LATFbits.LATF0 = !LATFbits.LATF0; 
            __delay_ms(500);
            LATFbits.LATF0 = !LATFbits.LATF0;
            __delay_ms(500);
        }        
        if (lightnumber == 5){                  // Buggy light 5 = TURN-R light
            LATHbits.LATH0 = !LATHbits.LATH0; 
            __delay_ms(500);
            LATHbits.LATH0 = !LATHbits.LATH0;
            __delay_ms(500);
        }    
    }
}

void color_click_init(void)
{   
                                        //setup colour sensor via i2c interface
    I2C_2_Master_Init();                //Initialise i2c Master
                                        //set device PON
	color_writetoaddr(0x00, 0x01);
    __delay_ms(3);                      //need to wait 3ms for everthing to start up                            
	color_writetoaddr(0x00, 0x03);      //turn on device ADC                            
	color_writetoaddr(0x01, 0xD5);      //set integration time
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();                //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);  //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();                 //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();                //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);     //command (auto-increment protocol transaction) + start at RED low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			 // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			 //read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();                 //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();                //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);     //command (auto-increment protocol transaction) + start at BLUE low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			 // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			 //read the Blue LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Blue MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();                 //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();                //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);     //command (auto-increment protocol transaction) + start at GREEN low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			 // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			 //read the Green LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Green MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();                 //Stop condition
	return tmp;
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();                //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);     //command (auto-increment protocol transaction) + start at CLEAR low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			 // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			 //read the Clear LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Clear MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();                 //Stop condition
	return tmp;
}

// Function used to read the color and store to a sturcture
void read_color (struct color_rgb *m)
{
    m->R = color_read_Red();             // Read red color
    m->B = color_read_Blue();            // Read blue color
    m->G = color_read_Green();           // Read green color
    m->C = color_read_Clear();           // Read white color (clear value)
}

// Function used to turn off all LED
void LED_OFF(void)
{    
    LATGbits.LATG1 = 0; // output LED_R set off (power)
    LATFbits.LATF7 = 0; // output LED_B set off (power)
    LATAbits.LATA4 = 0; // output LED_G set off (power)
    __delay_ms(200);
}

// Function used to turn on red LED
void LED_R(void)
{    
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 0; // output LED_B set off (power)
    LATAbits.LATA4 = 0; // output LED_G set off (power)
    __delay_ms(200);
}

// Function used to turn on white LED (all LEDs)
void LED_C(void)
{
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 1; // output LED_G set on (power)
    __delay_ms(200); 
}

// Function used to turn on blue LED (all LEDs)
void LED_B(void)
{
    LATGbits.LATG1 = 0; // output LED_R set off (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set off (power)
    __delay_ms(200);
}

// Function used to turn on green LED
void LED_G(void)
{    
    LATGbits.LATG1 = 0; // output LED_R set off (power)
    LATFbits.LATF7 = 0; // output LED_B set off (power)
    LATAbits.LATA4 = 1; // output LED_G set on (power)
    __delay_ms(200);
}

// Function used to compare the numerical values with a given upper bound and lower bound
unsigned char compare(unsigned int lower, unsigned int value2compare, unsigned int upper)
{
    unsigned char result = 0; 
    if (lower < value2compare && value2compare <= upper){result = 1;} // Return 1 if true, return 0 if false
    return result;
}


// Function used to detect color 
unsigned char detect_color(struct color_rgb *m, struct white_card *w)
{
    // Color code:
    // 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 0: black
    unsigned int RR = 0, RG = 0, RB = 0, GR = 0, GG = 0, GB = 0, BR = 0, BG = 0, BB = 0, BC = 0; // Initialise variables (see README for explanation of these variables)
    unsigned char color = 0; // Initialise a color; Color = 0 means zero
            
    LED_R();         // Turn on red light 
    __delay_ms(100);
    read_color(m);   // Read color
    // Calculate the ratio between the RGB data of the color card and the RGB data of the white card, when red light is on
    RR = lround((float)(m->R)/(w->RR)*100); RG = lround((float)(m->G)/(w->RG)*100); RB = lround((float)(m->B)/(w->RB)*100); 
    __delay_ms(50);
    
    LED_G();         // Turn on green light
    __delay_ms(100);
    read_color(m);   // Read color
    // Calculate the ratio between the RGB data of the color card and the RGB data of the white card, when green light is on
    GR = lround((float)(m->R)/(w->GR)*100); GG = lround((float)(m->G)/(w->GG)*100); GB = lround((float)(m->B)/(w->GB)*100);

    
    LED_B();        // Turn on blue light
    __delay_ms(100);
    read_color(m);  // Read color
    // Calculate the ratio between the RGB data of the color card and the RGB data of the white card, when blue light is on
    BR = lround((float)(m->R)/(w->BR)*100); BG = lround((float)(m->G)/(w->BG)*100); BB = lround((float)(m->B)/(w->BB)*100);
    BC = lround((float)(m->C)/(w->BC)*100);
    
    // Distinguish green (2) and blue (3)
    if (compare(0, RR, 45)){                                                 // If 0 < RR < 45
         if (compare(0, lround((float)(GG + BG)/BB * 200), 467)){color = 3;} // If 0 < (GG + BG)/ BB * 200 < 467, color = Blue
         else{color = 2;}                                                    // If (GG + BG) / BB * 200 > 467, color = Green
    }
    else{                                                                    // If RR >= 45
        if (GG <= 30 && GB <= 30){                                           // If GG <= 30 and GB <= 30
            // Distinguish red (1) and orange (6)
            if (compare(0, lround((float)GR/RR * 500), 150)){color = 1;}     // If 0 < (GR / RR) * 500 < 150, color = Red
            else {color = 6;}                                                // If (GR / RR) * 500 >= 150, color = Orange 
        }
        else if (GG > 30 && GB > 30){                                        // If GG > 30 and GB > 30
            // Distinguish yellow (4), pink (5) and light blue (7)
            if (BB >= 80){color = 7;}                                        // If BB >= 80, color = Light blue
            else{                                                            // If BB < 80
                if (BG < 48){color = 4;}                                     // If BG < 48, color = Yellow
                else {color = 5;}                                            // If BG >= 48, color = Pink
            }    
        }
        else {color = 0;}                                                    // Else, color = Black
    }
    // Distinguish white (8) and black (0)
    // If 90 < RR < RR * 2 and 90 < RB < RB * 2 and 90 < BG < BG * 2, color = white
    if (compare(90, RR, RR * 2) && compare(90, RB, RB * 2) && compare(90, BG, BG * 2)){color = 8;} 
    
    // If 0 < BR < 30 and 0 < BG < 30, color = black
    if (compare(0, BR, 30) && compare(0,BG,30)){color = 0;}
    
    if (color == 8) {toggle_light(2,1);} // If color = white, toggle buggy lights
    
    return color; // Return color
}

// Function used to display color readings to PC
void color_display(struct color_rgb *m)
{
    char buf[100];                                             // Define a string (character array)
    sprintf(buf,"%d\t%d\t%d\t%d\r\n", m->R, m->G, m->B, m->C); // Convert RGBC data to string
    sendStringSerial4(buf);                                    // Send to PC via serial
}

// Function used to display the predicted color to PC
void color_predict(unsigned char color)
{
    char buf[100];                                             // Define a string (character array)
    sprintf(buf,"\t%d\r\n", color);                            // Convert color code data to string
    sendStringSerial4(buf);                                    // Send to PC via serial
}

// Function used to calibrate the color with the white card and store into a structure
void calibrate_white(struct white_card *w)
{   
    LED_OFF();          // Turn off all lights to start with           
    toggle_light(1,2);  // Toggle buggy light
    __delay_ms(500);  
    
    LED_R();            // Turn on red light 
    __delay_ms(100);
    w->RR = color_read_Red(); w->RG = color_read_Green(); w->RB = color_read_Blue(); // Store RR, RG, RB data respectively (see README file for the explanation of these variables)
    
    LED_G();            // Turn on green light
    __delay_ms(100);
    w->GR = color_read_Red(); w->GG = color_read_Green(); w->GB = color_read_Blue(); // Store GR, GG, GB data respectively (see README file for the explanation of these variables)
    
    LED_B();            // Turn on blue light 
    __delay_ms(100);
    w->BR = color_read_Red(); w->BG = color_read_Green(); w->BB = color_read_Blue(); w->BC = color_read_Clear(); // Store BR, BG, BB, BC data respectively (see README file for the explanation of these variables)
    
    LED_C();            // Turn on white light
    __delay_ms(100);
    w->CR = color_read_Red(); w->CG = color_read_Green(); w->CB = color_read_Blue(); w->CC = color_read_Clear(); // Store CR, CG, CB, CC data respectively (see README file for the explanation of these variables)
    
    LED_OFF();          // Turn off all lights
    __delay_ms(500);
    toggle_light(1,2);  // Toggle the light
}

// Function used to collect data for testing
void color_data_collection(struct color_rgb *m)
{
    int i = 0;                // Initialise a counter that defines the number of repetitions of collecting the data
    for (i = 0; i < 1; ++i){
        LED_C();              // Turn on white light
        __delay_ms(100);
        read_color(m);        // Read data
        color_display(m);     // Display to PC

        LED_R();              // Turn on red light
        __delay_ms(100);
        read_color(m);        // Read data
        color_display(m);     // Display to PC

        LED_G();              // Similar procedures as above
        __delay_ms(100);
        read_color(m);
        color_display(m);
    
        LED_B();
        __delay_ms(100);
        read_color(m);
        color_display(m);
    }
    color_predict(1);         // Print number 1 as a end note, meaning that the data collection is completed
    LED_C();                  // Turn on all lights
}

// Function used to double check the color predicted
unsigned char verify_color(unsigned char color,struct color_rgb *m, struct white_card *w)
{
    __delay_ms(50);
    unsigned int color2; 
    color2 = detect_color(m,w);        // Detect the color the second time
    __delay_ms(50);
    if (color == color2){return color;}// If color detected in the first time = color detected in the second time, return color
    else {
        color = 0;
        return color;}                 // Else, return color = 0 (black)
}


// Function used to measure the ambient light in CC value
unsigned amb_light_measure(struct color_rgb *amb)
{
    unsigned int CC_amb_1 = 0, CC_amb_2 = 0, CC_amb_3 = 0, CC_amb_ave;
    toggle_light(2,2); // Toggle buggy lights
    LED_C();           // Turn on white light
    __delay_ms(500);
       
    CC_amb_1 = color_read_Clear(); // Read clear reading 
    __delay_ms(200);
    
    CC_amb_2 = color_read_Clear(); // Read clear reading 
    __delay_ms(200);

    CC_amb_3 = color_read_Clear(); // Read clear reading 
    __delay_ms(200);
    
    CC_amb_ave = lround((float)(CC_amb_1 + CC_amb_2 + CC_amb_3)/3); // Calculate the average reading

    toggle_light(2,2); // Toggle buggy lights                      
    
    return CC_amb_ave; // Return the averaged ambient light
}


// Function used to detect the presence of a wall or card
unsigned char detect_wall(struct DC_motor *mL, struct DC_motor *mR, unsigned int amb_light) 
{
    unsigned int CC_amb = 0;    // Initialise ambient light = 0
    unsigned char stop = 0;     // Initialise stop signal = 0
    unsigned int threshold = 0; // Initialise threshold = 0
    LED_C();                    // Turn on white light
    __delay_ms(100);
    CC_amb = color_read_Clear();// When the buggy is moving, it obtain the CC value from the ambient light
    threshold = lround((float)amb_light * 110 / 100); // The threshold is 1.1 times the ambient light measured by function 'amb_light_measure'

    if (CC_amb >= threshold){stop = 1;} // If the measured CC value is greater than the threshold, set stop signal to 1, instructing the buggy to stop
    return stop; // Return stop
}