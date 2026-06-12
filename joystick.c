/*
 * joystick.c
 *
 *  Created on: 28 abr. 2026
 *      Author: Max
 */

#include <msp430.h>
#include <stdint.h>

#include "I2C.h"
#include "delay.h"

void init_joystick(void)
{

    P4SEL1 &= ~(BIT5 | BIT6);
    P4SEL0 &= ~(BIT5 | BIT6);
    P4DIR &= ~(BIT5 | BIT6);        //Definimos los Pins 2.4 y 2.5 como entradas
    P4IE |= (BIT5 | BIT6);
    P4IFG &= ~(BIT5 | BIT6);
    P4IES |= (BIT5 | BIT6);   // flanco de bajada
    P4REN |= (BIT5 | BIT6);   // habilitar resistencias
    P4OUT |= (BIT5 | BIT6);   // pull-up

    P2SEL1 &= ~BIT5;
    P2SEL0 &= ~BIT5;
    P2DIR &= ~BIT5;        //Definimos los Pins 2.4 y 2.5 como entradas
    P2IE |= BIT5;
    P2IFG &= ~BIT5;
    P2IES |= BIT5;   // flanco de bajada
    P2REN |= BIT5;   // habilitar resistencias
    P2OUT |= BIT5;   // pull-up

    P3SEL1 &= ~(BIT4 | BIT5);
    P3SEL0 &= ~(BIT4 | BIT5);
    P3DIR &= ~(BIT4 | BIT5);        //Definimos los Pins 2.4 y 2.5 como entradas
    P3IE |= (BIT4 | BIT5);
    P3IFG &= ~(BIT4 | BIT5);
    P3IES |= (BIT4 | BIT5);   // flanco de bajada
    P3REN |= (BIT4 | BIT5);   // habilitar resistencias
    P3OUT |= (BIT4 | BIT5);   // pull-up
}

/*uint8_t joystickB5(void)
{
    uint8_t estado = 0;             //Variable donde se almacenará la dirección hacia donde se mueve el joystick

    //Condi
    if (!(P4IN & BIT5)) {
        estado = 0x00;            //estado joystick eje X hacia arriba
        I2C_send(0x10, engn_on, 5);
    }



    if (P4IN & BIT5) {
        estado = 0x02;          //estado joystick eje Y hacia arriba
        I2C_send(0x10, engn_back, 5);
    }

    return estado;
}*/






