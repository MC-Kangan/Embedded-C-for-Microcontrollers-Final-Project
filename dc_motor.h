#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000
#define TURNING_POWER_L 40 // This number needs to be adjusted according to different floor condition
#define TURNING_POWER_R 40 // This number needs to be adjusted according to different floor condition
#define FORWARD_POWER 70
#define SENSITIVITY 7      //define turning sensitivity

struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    unsigned char *dutyHighByte; //PWM duty high byte address
    unsigned char *dir_LAT; //LAT for dir pin
    char dir_pin; // pin number that controls direction on LAT
    int PWMperiod; //base period of PWM cycle
    char voltage;
};

//function prototypes
void initDCmotorsPWM(int PWMperiod); // function to setup PWM
void initDCmotors_parameter(struct DC_motor *motorL, struct DC_motor *motorR);
void setMotorPWM(struct DC_motor *m);
void stop(struct DC_motor *mL, struct DC_motor *mR);
void turnLeft(struct DC_motor *mL, struct DC_motor *mR, unsigned char angle_left);
void turnRight(struct DC_motor *mL, struct DC_motor *mR, unsigned char angle_right);
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR);
void turn180(struct DC_motor *mL, struct DC_motor *mR);
void voltage_read(struct DC_motor *m);
void voltage_display(struct DC_motor *m);

#endif