#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <xc.h>

//function prototypes
void action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR);
void test_action (struct DC_motor *mL, struct DC_motor *mR);
void pin_init(void);
void goback(struct DC_motor *mL, struct DC_motor *mR);

#endif