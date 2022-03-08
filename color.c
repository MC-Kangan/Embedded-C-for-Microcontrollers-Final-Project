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

void LED_R(struct color_rgb *m)
{    
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 0; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set on (power)
    __delay_ms(200);
    read_color(m);
}

void LED_C(struct color_rgb *m)
{
    LATGbits.LATG1 = 1; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 1; // output LED_G set on (power)
    __delay_ms(200);
    read_color(m);
}

void LED_B(struct color_rgb *m)
{
    LATGbits.LATG1 = 0; // output LED_R set on (power)
    LATFbits.LATF7 = 1; // output LED_B set on (power)
    LATAbits.LATA4 = 0; // output LED_G set on (power)
    __delay_ms(200);
    read_color(m);
}

void LED_G(struct color_rgb *m)
{    
    LATGbits.LATG1 = 0; // output LED_R set on (power)
    LATFbits.LATF7 = 0; // output LED_B set on (power)
    LATAbits.LATA4 = 1; // output LED_G set on (power)
    __delay_ms(200);
    read_color(m);
}

void color_display(struct color_rgb *m)
{
    char buf[100];
    sprintf(buf,"\t%d\t%d\t%d\t%d\r\n", m->R, m->G, m->B, m->C);
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
}


// Function used to detect color with white light
unsigned char detect_color(struct color_rgb *m)
{
    // Color code:
    // 1: red; 2: green; 3: blue; 4: yellow; 5:pink; 6:orange; 7:light blue; 8:white; 9: black
    
    unsigned int RR = 0, RG = 0, RB = 0, GR = 0, GG = 0, GB = 0, BR = 0, BG = 0, BB = 0;
    unsigned char color = 0;   
            
    LED_R(m); // Turn on red light 
    read_color(m); 
    RR = m->R; RG = m->G; RB = m->B;
    __delay_ms(50);
    
    LED_G(m); // Turn on green light
    read_color(m); 
    GR = m->R; GG = m->G; GB = m->B;
    __delay_ms(50);
    
    LED_B(m); // Turn on blue light
    read_color(m); 
    BR = m->R; BG = m->G; BB = m->B;
    __delay_ms(50);
    
    if (compare(RR, 600, 1)){ // if RR < 600
        if (compare(816, GG/BB*200, GG/BB*200 + 10)){color = 9;} // If 816 < GG/BB*2 <= GG/BB*2 + 10 (Define an upper bound randomly)
        else if (compare(742, GG/BB*200, 816 )){color = 2;} // If 742 < GG/BB*2 <= 816 
        else if (compare(0, GG/BB*200, 742 )){color = 3;} // If 0 < GG/BB*2 <= 742 (Define a lower bound randomly)
        else {color = 0;}
    } 
    else{
        if (compare(0, RG, 50)){ // If 0 < RG <= 50
            if (compare(0, RR/BG*100, 1597)) {color = 6;} 
            else if (compare(1597, RR/BG*100, RR/BG*100 + 10)) {color = 1;}
            else {color = 0;}
        }
        else if (compare(65, RG, RG + 10)){color = 8;} // If 65 < RG <= RG + 10
        else if (compare(50, RG, 65)){
            if (compare(80, BR, BR + 10)){color = 5;}
            else if (compare(0, BR, 80) && compare(0, RR/BG*100, 1075)){color = 7;}
            else if (compare(0, BR, 80) && compare(1075, RR/BG*100, RR/BR*100 + 10)){color = 4;}
            else {color = 0;}
        }
        else {color = 0;}
    }
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
    if (color == 1){turnRight(mL, mR); __delay_ms(500);}
}