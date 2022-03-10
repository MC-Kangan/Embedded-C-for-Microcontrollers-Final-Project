#ifndef _MOVEMENT_H
#define _MOVEMENT_H

#include <xc.h>

//function prototypes
void short_burst(struct DC_motor *mL, struct DC_motor *mR);
void action(unsigned char color, struct DC_motor *mL, struct DC_motor *mR);
void test_action (struct DC_motor *mL, struct DC_motor *mR);
void pin_init(void);
void count_time(void);
void goback(struct DC_motor *mL, struct DC_motor *mR);

#endif