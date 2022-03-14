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

void buggylight_init(void) //initiations for buggy lights
{
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


void toggle_light(unsigned char lightnumber, unsigned char toggletime)
{
    unsigned char i = 0;
    
    for (i = 0; i < toggletime; i++){
        if (lightnumber == 1){
            LATHbits.LATH1 = !LATHbits.LATH1; 
            __delay_ms(500);
            LATHbits.LATH1 = !LATHbits.LATH1;
            __delay_ms(500);
        }
        if (lightnumber == 2){
            LATDbits.LATD4 = !LATDbits.LATD4; 
            __delay_ms(500);
            LATDbits.LATD4 = !LATDbits.LATD4;
            __delay_ms(500);
        }
        if (lightnumber == 3){
            LATDbits.LATD3 = !LATDbits.LATD3; 
            __delay_ms(500);
            LATDbits.LATD3 = !LATDbits.LATD3;
            __delay_ms(500);
        }        
        if (lightnumber == 4){
            LATFbits.LATF0 = !LATFbits.LATF0; 
            __delay_ms(500);
            LATFbits.LATF0 = !LATFbits.LATF0;
            __delay_ms(500);
        }        
        if (lightnumber == 3){
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
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Blue LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Blue MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Green LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Green MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at CLEAR low register (Color Datasheet 11)
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Clear LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Clear MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

void read_color (struct color_rgb *m)
{
    m->R = color_read_Red();
    m->B = color_read_Blue();
    m->G = color_read_Green();
    m->C = color_read_Clear();
}

void LED_OFF(void)
{    
    LATGbits.LATG1 = 0; // output LED_R set on (power)
    LATFbits.LATF7 = 0; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set on (power)
    __delay_ms(200);
    //read_color(m);
}

void LED_R(void)//struct color_rgb *m)
{    
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 0; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set on (power)
    __delay_ms(200);
    //read_color(m);
}

void LED_C(void)//struct color_rgb *m)
{
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 1; // output LED_G set on (power)
    __delay_ms(50); // originally 200
    //read_color(m);
}

void LED_B(void)//struct color_rgb *m)
{
    LATGbits.LATG1 = 0; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set on (power)
    __delay_ms(200);//200
    //read_color(m);
}

void LED_G(void)//struct color_rgb *m)
{    
    LATGbits.LATG1 = 0; // output LED_R set on (power)
    LATFbits.LATF7 = 0; // output LED_B set on (power)
    LATAbits.LATA4 = 1; // output LED_G set on (power)
    __delay_ms(200);
    //read_color(m);
}

void color_data_collection(struct color_rgb *m){
    
    int i = 0; int j = 0; int k = 0; int x = 0;
    for (i = 0; i <5; ++i){
        LED_C();
        read_color(m);
        color_display(m);
        __delay_ms(100);
    }  
    color_predict(00000);
    for (j = 0; j < 5; ++j){
        LED_R();
        read_color(m);
        color_display(m);
        __delay_ms(100);
    }
    color_predict(00000);
    for (k = 0; k < 5; ++k){
        LED_G();
        read_color(m);
        color_display(m);
        __delay_ms(100);
    }
    color_predict(00000);
    for (x = 0; x < 5; ++x){
        LED_B();
        read_color(m);
        color_display(m);
        __delay_ms(100);
    }
    color_predict(1);
    LED_C();
}

void color_display(struct color_rgb *m)
{
    char buf[100];
    sprintf(buf,"%d\t%d\t%d\t%d\r\n", m->R, m->G, m->B, m->C);
    sendStringSerial4(buf);
}

void check_color_reading(struct color_rgb *m, struct white_card *w)
{
    char buf[100];
    
    int result = lround((float)(m->R)/(w->RR)*100);
    sprintf(buf,"\t%d\t%d\t%d\r\n", m->R, w->RR, result);
    sendStringSerial4(buf);
}

void color_predict(unsigned char color)
{
    char buf[100];
    sprintf(buf,"\t%d\r\n", color);
    sendStringSerial4(buf);
    //sendStringSerial4(color_name);
}

void calibrate_white(struct white_card *w)
{   
    LED_OFF();
    toggle_light(1,2);
    __delay_ms(500);
    
    LED_R(); // Turn on red light 
    w->RR = color_read_Red(); w->RG = color_read_Green(); w->RB = color_read_Blue();
    __delay_ms(100);
    
    LED_G(); // Turn on green light
    w->GR = color_read_Red(); w->GG = color_read_Green(); w->GB = color_read_Blue();
    //w->GC = color_read_Clear();
    __delay_ms(100);
    
    LED_B(); // Turn on blue light 
    w->BR = color_read_Red(); w->BG = color_read_Green(); w->BB = color_read_Blue(); w->BC = color_read_Clear(); 
    __delay_ms(100);
    
    LED_C(); // Turn on white light
    w->CR = color_read_Red(); w->CG = color_read_Green(); w->CB = color_read_Blue(); w->CC = color_read_Clear();
    __delay_ms(500);
    
    LED_OFF();
    __delay_ms(500);
    toggle_light(1,2);
}

// Function used to detect color with white light
unsigned char detect_color(struct color_rgb *m, struct white_card *w)
{
    
    
    // Color code:
    // 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 0: black
    unsigned int RR = 0, RG = 0, RB = 0, GR = 0, GG = 0, GB = 0, BR = 0, BG = 0, BB = 0, BC = 0; //GR_REAL = 0, GC_REAL = 0, BC = 0;
    unsigned char color = 0;   
            
    LED_R(); // Turn on red light 
    read_color(m); 
    //RR = (float)(m->R)/(w->RR)*100; RG = (float)(m->G)/(w->RG)*100; RB =(float)(m->B)/(w->RB)*100;
    RR = lround((float)(m->R)/(w->RR)*100); RG = lround((float)(m->G)/(w->RG)*100); RB = lround((float)(m->B)/(w->RB)*100);
    __delay_ms(50);
    char buf[100];
    
    sprintf(buf,"%d\t%d\t%d\r\n", RR, RG, RB);
    sendStringSerial4(buf);
    
    LED_G(); // Turn on green light
    read_color(m); 
    //GR_REAL = m->R ;  GC_REAL = m->C;
    //GR = (float)(m->R)/(w->GR)*100; GG = (float)(m->G)/(w->GG)*100; GB = (float)(m->B)/(w->GB)*100;
    GR = lround((float)(m->R)/(w->GR)*100); GG = lround((float)(m->G)/(w->GG)*100); GB = lround((float)(m->B)/(w->GB)*100);
    __delay_ms(50);

    sprintf(buf,"%d\t%d\t%d\r\n", GR, GG, GB);
    sendStringSerial4(buf);
    
    LED_B(); // Turn on blue light
    read_color(m); 
    BR = lround((float)(m->R)/(w->BR)*100); BG = lround((float)(m->G)/(w->BG)*100); BB = lround((float)(m->B)/(w->BB)*100);
    BC = lround((float)(m->C)/(w->BC)*100);
    __delay_ms(50);

    sprintf(buf,"%d\t%d\t%d\r\n", BR, BG, BB);
    sendStringSerial4(buf);
    
    // Distinguish green (2) and blue (3)
    if (compare(0, BR, 70)){ // if BR < 70
         if (compare(0, lround((float)(GG + BG)/BB * 200), 411)){color = 3;}// if (GG+BG)/BB*2 < 391 //Blue
        else{color = 2;} // if (GG+BG)/BB*2 > 391 //Green
    }
    else{
        if (compare(0, BG, 75)){ // if RG < 75
            // Distinguish red (1) and orange (6)
            if (compare(480, lround((float)GR/RR * 500), 520)){color = 6;}
            else {color = 1;} // if RR/BG*2 >= 313
        }
        else{ 
            // Distinguish yellow (4), pink (5) and light blue (7)
            if (BG > BR) {color = 7;}
            else{
                if (compare(490, lround((float)BG / BB * 500), 520)){color = 5;}
                if (lround((float)BG / BB * 500) > 520) {color = 4;}
                else {color = 0;}
            }    
        }
    }
    // Group 0 (black and white)
    if (compare(90, BR, BR * 30) && compare(90, BB, BR * 30)){color = 8;}
    if (compare(0, BR, 25) && compare(0,RR,90)){color = 0;}
    
    if (color == 8) {toggle_light(2,1);}

    return color;
}

unsigned char compare(unsigned int lower, unsigned int value2compare, unsigned int upper)
{
    unsigned char result = 0;
    if (lower < value2compare && value2compare <= upper){result = 1;} // Return 1 if true
    return result;
}

// Function used to check the color after detecting the color with white light
unsigned char verify_color(unsigned char color,struct color_rgb *m, struct white_card *w)
{
    __delay_ms(50);
    unsigned int color2, color3;
    color2 = detect_color(m,w);
    __delay_ms(50);
//    color3 = detect_color(m,w);
    if (color == color2){return color;}
    else {
        color = 0;
        return color;}
}

unsigned amb_light_measure(struct color_rgb *amb)
{
    unsigned int CC_amb_1 = 0, CC_amb_2 = 0, CC_amb_3 = 0, CC_amb_ave, upper_bound = 0;
    toggle_light(2,2);
    LED_C();
    __delay_ms(500);
       
    CC_amb_1 = color_read_Clear();
    __delay_ms(200);
    
    CC_amb_2 = color_read_Clear();
    __delay_ms(200);

    CC_amb_3 = color_read_Clear();
    __delay_ms(200);
    
    CC_amb_ave = lround((float)(CC_amb_1 + CC_amb_2 + CC_amb_3)/3);

    toggle_light(2,2);
    
    return CC_amb_ave;  
}

unsigned char distance_measure(struct DC_motor *mL, struct DC_motor *mR, unsigned int amb_light) 
{
    unsigned int CC_amb = 0, CG_amb = 0; 
    unsigned char stop = 0;
    unsigned int threshold = 0; // should be CC 
    LED_C();
    __delay_ms(100);
    CC_amb = color_read_Clear();
    CG_amb = color_read_Green();//, CR = m->R, CG = m->G, CB = m->B;
    threshold = lround((float)amb_light * 1.05);
    //threshold = lround((float)(w->CC)/ 105 * 100);

    if (CC_amb >= threshold){stop = 1;}
    return stop;
}