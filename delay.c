/*
 * delay.c
 *
 *  Created on: 6 abr. 2026
 *      Author: Max y Manuel
 */

#include <msp430.h>
#include <stdint.h>

volatile uint8_t seguir_2;               //control FG
volatile uint16_t contador;              //cuenta los ms que han pasado
volatile uint16_t valor_cont;            //numero  ms que queremos hacer el delay



void delay_ms(uint16_t msec)
{
    seguir_2 =1;                    //ponemos a 1 la control FG
    contador = 0;                  //reiniciamos el contador
    valor_cont = msec;            //valor de delay deseado por el usuario
    TB3CTL |= MC_1 | TBCLR;      //ponemos el TB3 a 0 y en modo start
    while(seguir_2);            //esperamos a que la ISR indique cuando se ha llegado al valor indicado por el usuario
    TB3CTL &= ~(MC_3);            //paramos el timer

}


#pragma vector=TIMER3_B0_VECTOR
__interrupt void ISR_delay_ms (void)
{
/*Esta ISR se ejecutará cada vez que el TB3 llegue a CCRO (16000 -> 1 ms)*/
    if(contador<valor_cont){        //contamos hasta  el valor decidido por el usuario
        contador=contador+1;        //augmentamos el contador
    }
    else {
        seguir_2 = 0;               //desactivamos la bandera de control
    }
}

