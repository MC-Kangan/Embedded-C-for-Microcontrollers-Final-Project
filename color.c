#include <xc.h>
#include "dc_motor.h"
#include "serial.h"
#include "color.h"
#include "i2c.h"
#include <stdio.h>
//#include "movement.h"

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
    __delay_ms(200);
    //read_color(m);
}

void LED_B(void)//struct color_rgb *m)
{
    LATGbits.LATG1 = 0; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set on (power)
    __delay_ms(200);
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

void color_display(struct color_rgb *m)
{
    char buf[100];
    sprintf(buf,"\t%d\t%d\t%d\t%d\r\n", m->R, m->G, m->B, m->C);
    sendStringSerial4(buf);
}

void check_color_reading(struct color_rgb *m, struct white_card *w)
{
    char buf[100];
    sprintf(buf,"\t%d\t%d\t%d\r\n", m->R, w->RR, 100*(m->R)/(w->RR));
    sendStringSerial4(buf);
}


void color_predict(unsigned char color)
{
    char buf[100];
    char color_name;
    if (color == 0){color_name = "Error";}
    if (color == 1){color_name = "Red";}
    if (color == 2){color_name = "Green";}
    if (color == 3){color_name = "Blue";}
    if (color == 4){color_name = "Yellow";}
    if (color == 5){color_name = "Pink";}
    if (color == 6){color_name = "Orange";}
    if (color == 7){color_name = "Light blue";}
    if (color == 8){color_name = "White";}
    if (color == 9){color_name = "Black";}
    
    sprintf(buf,"\t%d\r\n", color);
    sendStringSerial4(buf);
    //sendStringSerial4(color_name);
}

void calibrate_white(struct white_card *w)
{
    LED_R(); // Turn on red light 
    read_color(w); 
    w->RR = color_read_Red(); w->RG = color_read_Green(); w->RB = color_read_Blue();
    __delay_ms(50);
    
    LED_G(); // Turn on green light
    read_color(w); 
    w->GR = color_read_Red(); w->GG = color_read_Green(); w->GB = color_read_Blue();
    __delay_ms(50);
    
    LED_B(); // Turn on blue light
    read_color(w); 
    w->BR = color_read_Red(); w->BG = color_read_Green(); w->BB = color_read_Blue();
    __delay_ms(50);
}

// Function used to detect color with white light
unsigned char detect_color(struct color_rgb *m, struct white_card *w)
{
    // Color code:
    // 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
    unsigned int RR = 0, RG = 0, RB = 0, GR = 0, GG = 0, GB = 0, BR = 0, BG = 0, BB = 0;
    unsigned char color = 0;   
            
    LED_R(); // Turn on red light 
    read_color(m); 
    RR = 100*(m->R)/(w->RR); RG = 100*(m->G)/(w->RG); RB = 100*(m->B)/(w->RB);
    __delay_ms(50);
    
    LED_G(); // Turn on green light
    read_color(m); 
    GR = 100*(m->R)/(w->GR); GG = 100*(m->G)/(w->GG); GB = 100*(m->B)/(w->GB);
    __delay_ms(50);
    
    LED_B(); // Turn on blue light
    read_color(m); 
    BR = 100*(m->R)/(w->BR); BG = 100*(m->G)/(w->BG); BB = 100*(m->B)/(w->BB);
    __delay_ms(50);
    
    
    if (compare(0, RR, 90)){ // if RR < 90
        if (compare(0, GG/BB*200, 228)){color = 3;} //Blue
        else{color = 2;} //Green
    }
    else{
        if (compare(0,RG,80)){
            if (compare(0, RR/BG*200, 319)){color = 6;}
            else {color = 1;}
        }
        else{
            if (compare(0, BR, 95)){color = 7;}
            else{
                if (BG < BB){color = 5;}
                else{color = 4;}
            }
        }
    }
    // Group 0 (black and white)
    if (compare(95, BR, 105) && compare(95,BG,105)){color = 8;}
    if (compare(0, BR, 20) && compare(0,RR,90)){color = 9;}
    return color;
}

unsigned char compare(unsigned int lower, unsigned int value2compare, unsigned int upper)
{
    unsigned char result = 0;
    if (lower < value2compare && value2compare <= upper){result = 1;} // Return 1 if true
    return result;
}

// Function used to check the color after detecting the color with white light
unsigned char check_color(unsigned char color,struct color_rgb *m)
{
    return color;
}

void movement (unsigned char color,struct DC_motor *mL, struct DC_motor *mR)
{
    if (color == 1){turnRight(mL, mR,90); __delay_ms(500);}
}