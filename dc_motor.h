#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define TURNING_POWER 30 // This number needs to be adjusted according to different floor condition
#define FORWARD_POWER_L 34
#define FORWARD_POWER_R 30
#define BACKWARD_POWER 40
unsigned int SENSITIVITY = 355;

struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    unsigned char *dutyHighByte; //PWM duty high byte address
    unsigned char *dir_LAT; //LAT for dir pin
    char dir_pin; // pin number that controls direction on LAT
    int PWMperiod; //base period of PWM cycle
};

//function prototypes
void initDCmotorsPWM(int PWMperiod); // function to setup PWM
void initDCmotors_parameter(struct DC_motor *motorL, struct DC_motor *motorR);
void setMotorPWM(struct DC_motor *m);
void stop(struct DC_motor *mL, struct DC_motor *mR);
void turn45(struct DC_motor *mL, struct DC_motor *mR, unsigned char turn_time, unsigned char direction);
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR);
void fullSpeedAhead_test(struct DC_motor *mL, struct DC_motor *mR);
void fullSpeedBack(struct DC_motor *mL, struct DC_motor *mR, unsigned char instruction);

#endif