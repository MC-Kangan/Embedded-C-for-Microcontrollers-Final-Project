#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <xc.h>

unsigned int second = 0;
unsigned int memory[20];
unsigned char array_index = 0;

//function prototypes
void short_burst(struct DC_motor *mL, struct DC_motor *mR);
void action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR);
void test_action (struct DC_motor *mL, struct DC_motor *mR);
void pin_init(void);
void goback(struct DC_motor *mL, struct DC_motor *mR);

#endif