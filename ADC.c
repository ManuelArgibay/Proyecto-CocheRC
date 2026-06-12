/*
 * ADC.c
 *
 *  Created on: May 14, 2026
 *      Author: manuel
 */

#include <msp430.h>
#include <stdint.h>

#define A8 ADCINCH_8
#define A4 ADCINCH_4

void init_ADC(void){
    PM5CTL0 &= ~LOCKLPM5;                  // to activate previously configured port settings
    P1SEL0 |= BIT4;                        // Enable A/D channel inputs
    P1SEL1 &= ~(BIT4);                    // Enable A/D channel inputs
    P5SEL0 |= BIT0;                        // Enable A/D channel inputs
    P5SEL1 &= ~(BIT0);                    // Enable A/D channel inputs

    ADCCTL0 |= ADCON+ADCSHT_2; // Turn on ADC12, S&H=16 clks
    ADCCTL1 |= ADCSHP;       // ADCLK=MODOSC,
    ADCCTL2 &= ~ADCRES;     // clear ADCRES in ADCTL
    ADCCTL2 |= ADCRES_1;   // 10 bits resolution
    ADCCTL0 &= ~ADCENC;

}


uint16_t leer_ADC(uint16_t canal){
    ADCCTL0 &= ~ADCENC;
    ADCMCTL0 = canal;
    ADCCTL0 |= ADCENC | ADCSC;
    while(ADCCTL1 & ADCBUSY);
    return ADCMEM0;
}

