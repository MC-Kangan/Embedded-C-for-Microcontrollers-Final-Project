#include <xc.h>
#include "dc_motor.h"
#include "serial.h"
#include <stdio.h>
#include "movement.h"
#include "test_and_calibration.h"
// function initialise T2 and PWM for DC motor control
struct DC_motor motorL, motorR;

void initDCmotorsPWM(int PWMperiod){
	//initialise your TRIS and LAT registers for PWM
    
    TRISEbits.TRISE2=0; //output on RE2
    TRISCbits.TRISC7=0; //output on RC7 
    TRISGbits.TRISG6=0; //output on RG6 
    TRISEbits.TRISE4=0; //output on RE4 
    LATEbits.LATE2=0; // output on RE2 set low (power)
    LATCbits.LATC7=0; // output on RC7 set low (power)
    LATGbits.LATG6=0; // output on RG6 set high (direction: forward) right motor
    LATEbits.LATE4=0; // output on RE4 set high (direction: forward) left  motor
 
    // timer 2 config
    T2CONbits.CKPS=0b011; // 1:8 prescaler, according to calculation
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    T2PR=200-1; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    RE2PPS=0x0A; //set PWM6 on RE2 (power pin), thus RE4 is direction pins for left motor
    RC7PPS=0x0B; //set PMW7 on RC7 (power pin), thus RG6 is the direction pins for right motor

    PWM6DCH=0; //0% power (PWM Duty Cycle Register, P436)
    PWM7DCH=0; //0% power (PWM Duty Cycle Register, P436)
    
    PWM6CONbits.EN = 1;
    PWM7CONbits.EN = 1;
}
void initDCmotors_parameter(struct DC_motor *motorL, struct DC_motor *motorR){
   // For the motor on the left
    motorL->power = 0;                                  // Set power to 0
    motorL->direction = 1;                              // Set direction to 1 (forward)
    motorL->dutyHighByte = (unsigned char *)(&PWM6DCH); // Store address of PWM duty high byte
    motorL->dir_LAT = (unsigned char *)(&LATE);
    motorL->dir_pin = 4;                                // With dir_lat above, the direction pin for left motor is E4
    motorL->PWMperiod = 199;                            // Same as T2PR defined in dc_motor.c
    
    // For the motor on the right
    motorR->power = 0;                                  // Set power to 0
    motorR->direction = 1;                              // Set direction to 1 (forward)
    motorR->dutyHighByte = (unsigned char *)(&PWM7DCH); // Store address of PWM duty high byte
    motorR->dir_LAT = (unsigned char *)(&LATG);
    motorR->dir_pin = 6;                                // With dir_lat above, the direction pin for left motor is G6
    motorR->PWMperiod = 199;                            // Same as T2PR defined in dc_motor.c
}

// function to set PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
	int PWMduty;                                                //tmp variable to store PWM duty cycle
	if (m->direction){                                          //if forward (When direction channel is high, the power is controlled by PWM off period)
                                                                // low time increases with power
		PWMduty=m->PWMperiod - ((int)(m->power)*(m->PWMperiod))/100;
	}
	else {                                                      //if reverse (When direction channel is low, the power is controlled by PWM on period)
                                                                // high time increases with power
		PWMduty=((int)(m->power)*(m->PWMperiod))/100;
	}
	*(m->dutyHighByte) = PWMduty;                               //set high duty cycle byte    
	if (m->direction){                                          // if direction is high
		*(m->dir_LAT) = *(m->dir_LAT) | (1<<(m->dir_pin));      // set dir_pin bit in LAT to high without changing other bits
	} else {
		*(m->dir_LAT) = *(m->dir_LAT) & (~(1<<(m->dir_pin)));   // set dir_pin bit in LAT to low without changing other bits
	}
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
    while (mL->power >0 | mR->power >0){       // when either of the powers for left and right motors are larger than 0
        if (mL->power !=0){mL->power -= 2;}    // reduce the left motor power by 2 for each time (this case the power of motor doesn't drop to 0 immediately)
        if (mR->power !=0){mR->power -= 2;}    // reduce the right motor power by 2 for each time (this case the power of motor doesn't drop to 0 immediately)
        setMotorPWM(mL);                       // set the power to motor
        setMotorPWM(mR);                       // set the power to motor
        __delay_ms(5);
    }
    __delay_ms(500);    // buggy will stop there for 0.5 seconds after the motor is not moving
}

void turn45(struct DC_motor *mL, struct DC_motor *mR, unsigned char turn_time, unsigned char direction)
{
    // Direction 1: left; Direction 2: right
    unsigned char i = 0;
    for (i = 0; i < turn_time; i++){
        mL->direction=1;                           //set forward direction
        mR->direction=1;                           //set forward direction
        if (direction == 1){                       // turn left
            while (mR->power <TURNING_POWER){      // when the right motor power is lower than the setting value(see dc_motor.h)
                mR->power += 5;                    // increase the right motor power by 5 for each time (this case the power of motor doesn't push to the setting value immediately)
                mL->power = 0;                     // keep the left motor static, so the car will turn left               
                setMotorPWM(mL);                   // set the power to motor
                setMotorPWM(mR);                   // set the power to motor
                __delay_ms(10);
            } 
            for(unsigned int i = 0; i<SENSITIVITY; i++){__delay_ms(1);}    // delay time for turning: control how far the buggy will turn left. see sensitivity value in dc_motor.h
            stop(mL,mR);                           // stop the movement of the buggy for 0.5 second after turning
        }
        else if (direction == 2){                  // turn right
            while (mL->power <TURNING_POWER){      // when the right motor power is lower than the setting value(see dc_motor.h)
                mL->power += 5;                    // increase the left motor power by 5 for each time (this case the power of motor doesn't push to the setting value immediately)
                mR->power = 0;                     // keep the right motor static, so the car will turn right              
                setMotorPWM(mL);                   // set the power to motor
                setMotorPWM(mR);                   // set the power to motor
                __delay_ms(10);
            }
            for(unsigned int i = 0; i <SENSITIVITY + 10; i++){__delay_ms(1);}   // delay time for turning: control how far the buggy will turn right. see sensitivity value in dc_motor.h
            stop(mL,mR);                           // stop the movement of the buggy for 0.5 second after turning
        }
    }
} 

//function to make the buggy go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction=1;                                                    // set forward direction
    mR->direction=1;                                                    // set forward direction
    while (mL->power<FORWARD_POWER_L | mR->power<FORWARD_POWER_R){      // when the powers for left and right motors are smaller than the setting value(see dc_motor.h)
        if (mL->power<FORWARD_POWER_L) {mL->power += 2;}                // increase the left motor power by 2 for each time (this case the power of motor doesn't push to the setting value immediately)
        if (mR->power<FORWARD_POWER_R) {mR->power += 2;}                // increase the right motor power by 10 for each time (this case the power of motor doesn't push to the setting value immediately)                                             
        setMotorPWM(mL);                                                // set the power to motor
        setMotorPWM(mR);                                                // set the power to motor
        __delay_ms(10);
    }
}

void fullSpeedAhead_test(struct DC_motor *mL, struct DC_motor *mR)
{   // a function for testing, just easier to call. Don't have to write delay and stop all the time in the testing script
    fullSpeedAhead(mL,mR);  // full-speed ahead for a particular time
    __delay_ms(3000);       // full-speed ahead for 3 seconds
    stop(mL,mR);            // stop the movement of the buggy for 0.5 second after full-speed ahead
}

void fullSpeedBack(struct DC_motor *mL, struct DC_motor *mR, unsigned char instruction)
{
    mL->direction=0;                                                // set backward direction
    mR->direction=0;                                                // set backward direction
    while (mL->power<BACKWARD_POWER | mR->power<BACKWARD_POWER){    // when the powers for left and right motors are smaller than the setting value(see dc_motor.h)
        mL->power += 10;                                            // increase the left motor power by 10 for each time (this case the power of motor doesn't push to the setting value immediately)
        mR->power += 10;                                            // increase the right motor power by 10 for each time (this case the power of motor doesn't push to the setting value immediately)
        setMotorPWM(mL);                                            // set the power to motor
        setMotorPWM(mR);                                            // set the power to motor
        __delay_ms(10);
    }
    // full-speed back for a particular time 
    if (instruction == 1) {__delay_ms(600);}        // short delay, short reverse
    if (instruction == 2) {__delay_ms(800);}        // medium delay, medium reverse
    if (instruction == 3) {__delay_ms(1400);}       // large delay, large reverse
    stop(mL,mR);                                    // stop the movement of the buggy for 0.5 second after reverse
}