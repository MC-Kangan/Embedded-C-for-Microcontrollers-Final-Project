#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define TURNING_POWER 50        // This number controls the turning power
#define FORWARD_POWER_L 30      // This number controls the left motor power when moving forward
#define FORWARD_POWER_R 30      // This number controls the right motor power when moving forward
#define BACKWARD_POWER 44       // This number controls the backward power
unsigned int SENSITIVITY = 150; // This number controls the turning delay time

struct DC_motor {                   //definition of DC_motor structure
    char power;                     //motor power, out of 100
    char direction;                 //motor direction, forward(1), reverse(0)
    unsigned char *dutyHighByte;    //PWM duty high byte address
    unsigned char *dir_LAT;         //LAT for dir pin
    char dir_pin;                   //pin number that controls direction on LAT
    int PWMperiod;                  //base period of PWM cycle
};

//function prototypes
void initDCmotorsPWM(int PWMperiod);
void initDCmotors_parameter(struct DC_motor *motorL, struct DC_motor *motorR);
void setMotorPWM(struct DC_motor *m);
void stop(struct DC_motor *mL, struct DC_motor *mR);
void turn45(struct DC_motor *mL, struct DC_motor *mR, unsigned char turn_time, unsigned char direction);
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR);
void fullSpeedBack(struct DC_motor *mL, struct DC_motor *mR, unsigned char instruction);

#endif