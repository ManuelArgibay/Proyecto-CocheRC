/*
 * joystick.h
 *
 *  Created on: 28 abr. 2026
 *      Author: Max
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include <msp430.h>
#include <stdint.h>

uint8_t joystickB4(void);
uint8_t joystickB5(void);

void init_joystick(void);



#endif /* JOYSTICK_H_ */
