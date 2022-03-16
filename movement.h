#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <xc.h>

unsigned int centisecond = 0; //1 centisecond = 100ms
unsigned int memory[20];
unsigned char array_index = 0;

//function prototypes

void short_burst(struct DC_motor *mL, struct DC_motor *mR);
unsigned int straight_action(struct DC_motor *mL, struct DC_motor *mR, unsigned int amb_light, unsigned int start_time, unsigned int stop_time);
void distance_memory(struct DC_motor *mL, struct DC_motor *mR, unsigned int duration, unsigned char accident);
void turning_action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR);
void goback(struct DC_motor *mL, struct DC_motor *mR);

#endif