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

struct white_card { //definition of white_card structure
    unsigned int RR ;         //Red color reading when red light on
    unsigned int RG ;         //Green color reading when red light on
    unsigned int RB;         //Blue color reading when red light on
    unsigned int GR ;         //Red color reading when green light on
    unsigned int GG ;         //Green color reading when green light on
    unsigned int GB ;         //Blue color reading when green light on
    unsigned int BR ;         //Red color reading when blue light on
    unsigned int BG ;         //Green color reading when blue light on
    unsigned int BB ;         //Blue color reading when blue light on
    //unsigned int GC ;         //Clear color reading when green light on
    unsigned int CR ;         //Red color reading when white light on
    unsigned int CG ;         //Green color reading when white light on
    unsigned int CB ;         //Blue color reading when white light on
    unsigned int CC ;         //White color reading when white light on
    
};


void buggylight_init(void);

void toggle_light(unsigned char lightnumber, unsigned char toggletime);

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
void read_color (struct color_rgb *m);
void LED_OFF(void);
void LED_R(void);//struct color_rgb *m);
void LED_C(void);//(struct color_rgb *m);
void LED_B(void);//(struct color_rgb *m);
void LED_G(void);//(struct color_rgb *m);
void color_display(struct color_rgb *m);
void calibrate_white(struct white_card *w);
void color_predict(unsigned char color);
unsigned char detect_color(struct color_rgb *m, struct white_card *w);
unsigned char verify_color(unsigned char color,struct color_rgb *m, struct white_card *w);
unsigned char compare(unsigned int lower, unsigned int value2compare, unsigned int upper);

void check_color_reading(struct color_rgb *, struct white_card *w);
void color_data_collection(struct color_rgb *m);
//unsigned char distance_measure(struct DC_motor *mL, struct DC_motor *mR, struct white_card *w);
unsigned char distance_measure(struct DC_motor *mL, struct DC_motor *mR, unsigned int amb_light) ;
unsigned amb_light_measure(struct color_rgb *amb);

#endif
