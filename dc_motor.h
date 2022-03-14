#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>
//#include "test_and_calibration.h"
//#include "color.h"

#define _XTAL_FREQ 64000000
#define TURNING_POWER_L 25 // This number needs to be adjusted according to different floor condition
#define TURNING_POWER_R 25 // This number needs to be adjusted according to different floor condition
#define FORWARD_POWER 20
#define BACKWARD_POWER 50
unsigned char CALIBRATION_135 = 15;
unsigned char CALIBRATION_180 = 10;
unsigned char SENSITIVITY = 10.5;

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
void halfSpeedBack(struct DC_motor *mL, struct DC_motor *mR);
void turnLeft(struct DC_motor *mL, struct DC_motor *mR, unsigned char angle_left);
void turnRight(struct DC_motor *mL, struct DC_motor *mR, unsigned char angle_right);
void fullSpeedAhead(struct DC_motor *mL, struct DC_motor *mR);
void fullSpeedAhead_test(struct DC_motor *mL, struct DC_motor *mR);
void fullSpeedBack(struct DC_motor *mL, struct DC_motor *mR);
void short_reverse(struct DC_motor *mL, struct DC_motor *mR);
void reverse_square(struct DC_motor *mL, struct DC_motor *mR);


#endif