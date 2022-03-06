#include <xc.h>
#include "dc_motor.h"
#include "ADC.h"
#include "serial.h"
#include <stdio.h>
// function initialise T2 and PWM for DC motor control
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
    motorL->power = 0; // Set power to 0
    motorL->direction = 1; // Set direction to 1 (forward)
    motorL->dutyHighByte = (unsigned char *)(&PWM6DCH); // Store address of PWM duty high byte
    motorL->dir_LAT = (unsigned char *)(&LATE);
    motorL->dir_pin = 4; // With dir_lat above, the direction pin for left motor is E4
    motorL->PWMperiod = 199; // Same as T2PR defined in dc_motor.c
    
    // For the motor on the right
    motorR->power = 0; // Set power to 0
    motorR->direction = 1; // Set direction to 1 (forward)
    motorR->dutyHighByte = (unsigned char *)(&PWM7DCH); // Store address of PWM duty high byte
    motorR->dir_LAT = (unsigned char *)(&LATG);
    motorR->dir_pin = 6; // With dir_lat above, the direction pin for left motor is G6
    motorR->PWMperiod = 199; // Same as T2PR defined in dc_motor.c
}

// function to set PWM output from the values in the motor structure
void setMotorPWM(struct DC_motor *m)
{
	int PWMduty; //tmp variable to store PWM duty cycle

	if (m->direction){ //if forward (When direction channel is high, the power is controlled by PWM off period)
		// low time increases with power
		PWMduty=m->PWMperiod - ((int)(m->power)*(m->PWMperiod))/100;
	}
	else { //if reverse (When direction channel is low, the power is controlled by PWM on period)
		// high time increases with power
		PWMduty=((int)(m->power)*(m->PWMperiod))/100;
	}

	*(m->dutyHighByte) = PWMduty; //set high duty cycle byte
        
	if (m->direction){ // if direction is high
		*(m->dir_LAT) = *(m->dir_LAT) | (1<<(m->dir_pin)); // set dir_pin bit in LAT to high without changing other bits
	} else {
		*(m->dir_LAT) = *(m->dir_LAT) & (~(1<<(m->dir_pin))); // set dir_pin bit in LAT to low without changing other bits
	}
}

//function to stop the robot gradually 
void stop(struct DC_motor *mL, struct DC_motor *mR)
{
    while (mL->power >0 || mR->power >0){   // when the powers for left and right motors are larger than 0
        if (mL->power !=0){mL->power -= 5;}                    // reduce the left motor power by 10 for each time (this case the power of motor doesn't drop to 0 immediately)
        if (mR->power !=0){mR->power -= 5;}                    // reduce the right motor power by 10 for each time (this case the power of motor doesn't drop to 0 immediately)
        setMotorPWM(mL);                    // set the power to motor
        setMotorPWM(mR);                    // set the power to motor
        __delay_ms(10);
    }
}

//function to make the robot turn left 
void turnLeft(struct DC_motor *mL, struct DC_motor *mR)
{
      mL->direction=1;
      mR->direction=1;
      while (mR->power <TURNING_POWER_R){      // when the right motor power is lower than the setting value(see dc_motor.h)
        mR->power += 5;                        // increase the right motor power by 1 for each time (this case the power of motor push to the setting value immediately)
        mL->power = 0;                         // keep the left motor static, so the car will turn left
        setMotorPWM(mR);                       // set the power to motor
        setMotorPWM(mL);
        __delay_ms(10);
    }
}

//function to make the robot turn right
void turnRight(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction=1;
    mR->direction=1;
    while (mL->power <TURNING_POWER_L){      // when the left motor power is lower than the setting value(see dc_motor.h)
        mL->power += 5;                      // increase the left motor power by 1 for each time (this case the power of motor push to the setting value immediately)
        mR->power = 0;                       // keep the right motor static, so the car will turn right
        setMotorPWM(mL);                     // set the power to motor
        setMotorPWM(mR);
        __delay_ms(10);
    }
}

//function to make the robot go straight
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction=1;
    mR->direction=1;
    mL->power = 0; // set the power for both motor to be 0 to start with, otherwise, due to previous movement, they might have different speed.
    mR->power = 0;
    while (mL->power<FORWARD_POWER && mR->power<FORWARD_POWER){         // when the powers for left and right motors are smaller than 70
        mL->power += 10;                                                // increase the left motor power by 10 for each time (this case the power of motor push to the setting value immediately)
        mR->power += 10;                                                // increase the right motor power by 10 for each time (this case the power of motor push to the setting value immediately)
        setMotorPWM(mL);                                                // set the power to motor
        setMotorPWM(mR);                                                // set the power to motor
        __delay_ms(10);
    }
}

//function to make the robot turn right 180 degree
void turn180(struct DC_motor *mL, struct DC_motor *mR)
{
    mL->direction=1;
    mR->direction=1;
    while (mR->power <TURNING_POWER_R + 5){
        mR->power += 1;
        mL->power = 0;
        setMotorPWM(mL);
        setMotorPWM(mR);                             // set the power to motor
        __delay_ms(10);
    }
}

void voltage_read(struct DC_motor *m)
{   m->voltage = ADC_getval();
}

void voltage_display(struct DC_motor *m)
{
    char buf[80];
    sprintf(buf,"%d\r\n", m->voltage);
    sendStringSerial4(buf);
}
