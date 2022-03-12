/* 
 * File:   test_and_calibration.h
 * Author: Kangan Chen
 *
 * Created on March 12, 2022, 5:36 PM
 */

#ifndef TEST_AND_CALIBRATION_H
#define	TEST_AND_CALIBRATION_H

#include <xc.h>
#include "dc_motor.h"
#include "color.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void calibration(struct DC_motor *mL, struct DC_motor *mR);
void test_function(unsigned char test_code, struct color_rgb *m, struct white_card *w, struct DC_motor *mL, struct DC_motor *mR);

#endif	/* TEST_AND_CALIBRATION_H */

