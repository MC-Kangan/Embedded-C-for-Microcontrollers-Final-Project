#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

struct color_rgb { //definition of color_rgb structure
    unsigned int R ;         //Red color reading
    unsigned int G ;         //Green color reading
    unsigned int B ;         //Blue color reading
    unsigned int C ;         //Clear color reading
};

/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Red(void);

/********************************************//**
 *  Function to read the blue channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Blue(void);

/********************************************//**
 *  Function to read the green channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Green(void);
unsigned int color_read_Clear(void);
void get_color (struct color_rgb *m);
void LED_R(struct color_rgb *m);
void LED_C(struct color_rgb *m);
void LED_B(struct color_rgb *m);
void LED_G(struct color_rgb *m);
void color_display(struct color_rgb *m);
void color_predict(unsigned char color);
unsigned char detect_color_C(struct color_rgb *m);
unsigned char check_color(unsigned char color,struct color_rgb *m);
unsigned char compare(unsigned int value2compare, unsigned int upper, unsigned int lower );
void movement (unsigned char color,struct DC_motor *mL, struct DC_motor *mR);
#endif
