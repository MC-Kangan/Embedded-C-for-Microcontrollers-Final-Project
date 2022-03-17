#ifndef TEST_AND_CALIBRATION_H
#define	TEST_AND_CALIBRATION_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 

unsigned int setup(struct white_card *white,struct color_rgb*amb,struct DC_motor *mL, struct DC_motor *mR);
void test_action(struct DC_motor *mL, struct DC_motor *mR);

void calibration_motor(struct DC_motor *mL, struct DC_motor *mR);
void test_function(unsigned char test_code, struct color_rgb *m, struct white_card *w, struct DC_motor *mL, struct DC_motor *mR);

#endif

