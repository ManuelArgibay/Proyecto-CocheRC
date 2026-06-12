/*
 * ADC.h
 *
 *  Created on: May 14, 2026
 *      Author: manuel
 */

#ifndef ADC_H_
#define ADC_H_

#include <msp430.h>
#include <stdint.h>

void init_ADC(void);
uint16_t leer_ADC(uint16_t canal);

#endif /* ADC_H_ */
