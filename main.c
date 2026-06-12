#include <msp430.h>             //libreria donde se encuentran las definiciones de nuestro microcontrolador
#include <stdint.h>             //definiciones estandard de enteros
#include <stdlib.h>
#include "clocks_timers.h"      //permite usar funciones de inicialización de timers y clocks
#include "I2C.h"                //permite usar funciones de inicialización y comunicación I2C, así como inicialización LCD
#include "delay.h"              //Definición de función para aplicar un delay de ms
#include "joystick.h"           //funciones relativas al joystick
#include "ADC.h"
#include <stdio.h>   // para sprintf
#include <string.h>  // para strlen

/*
 * I2C.h
 *
 *  Created on: 6 abr. 2026
 *      Author: Max y Manuel
 */

#define A8 ADCINCH_8
#define A4 ADCINCH_4

// --- VARIABLES PARA EL MOTOR ---
uint8_t engn_on[5] = {0x00, 1, 255, 1, 255};                //trama para poder mover el robot hacia delante
uint8_t engn_forward[5] = {0x00, 1, 100, 1, 100};                //trama para poder mover el robot hacia delante
uint8_t engn_on_turn_right[5] = {0x00, 1, 50, 2, 50};       //trama para poder girar el robot hacia la derecha
uint8_t engn_right_soft[5] = {0x00, 1, 0, 1, 100};
uint8_t engn_right_hard[5] = {0x00, 1, 0, 1, 255};
uint8_t engn_on_turn_left[5] = {0x00, 2, 50, 1, 50};        //trama para poder girar el robot hacia la izquierda
uint8_t engn_left_soft[5] = {0x00, 1, 100, 1, 0};           // Trama que indica giro suave hacia la izquierda
uint8_t engn_left_hard[5] = {0x00, 1, 255, 1, 0};           // Trama que indica giro fuerte hacia la izquierda
uint8_t engn_back[5] = {0x00, 2, 255, 2, 255};              //trama para ir marcha atrás
uint8_t engn_off[5] = {0x00, 1, 0, 1, 0};                   //trama para apagar los motores

// --- VARIABLES PARA EL LED ---
uint8_t led_on[3]  = {0x0B, 7, 7};
uint8_t led_off[3] = {0x0B, 0, 0};                          //trama para poder apagar los leds
uint8_t led_on_forward[3]  = {0x0B, 5, 5};
uint8_t led_on_turn_right[3]  = {0x0B, 0, 3};
uint8_t led_on_turn_left[3]  = {0x0B, 3, 0};
uint8_t led_on_back[3] = {0x0B, 4, 4};

volatile uint8_t menu_state = 0;                            //variable donde se definirá el estado del switch-case que controla los menus.
volatile uint8_t menu = 0;                                  //indica si esá activo el mený en el que estamos
volatile uint8_t motorjoystick = 0;                         //indica si el joystick esta en modo navegacion de menus o para mover el robot
volatile uint8_t modo_menu = 0;                             //indica si estamos navegando por los menus

volatile uint8_t flag_B4 = 0;
volatile uint8_t flag_B5 = 0;
volatile uint8_t flag_B2 = 0;
volatile uint8_t flag_B3 = 0;
volatile uint8_t flag_B1 = 0;
volatile uint8_t flag_menu_down = 0;                              //bandera para movernos hacia el siguiente menu
volatile uint8_t flag_menu_up = 0;                                //bandera para movernos hacia el anterior menu

void init_UART(void){
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0MCTLW = UCOS16;
    UCA0BRW = 8;
    UCA0MCTLW |= (10 << 4);
    UCA0MCTLW |= (0xF7 << 8);
    P1SEL0 |= BIT7 | BIT6;
    P1SEL1 &= ~(BIT7 | BIT6);
    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
    UCA0IFG &= ~UCRXIFG;
}


void ldrs(void){

    uint16_t valor_der;
    uint16_t valor_izq;
    uint16_t diferencia;

    while(!modo_menu){

        valor_der = leer_ADC(A8);
        valor_izq = leer_ADC(A4);

        if(valor_der > valor_izq){
            diferencia = valor_der - valor_izq;
            if(diferencia > 255){
                I2C_send(0x10, engn_right_hard, 5);
            }
            else if(diferencia < 30){
                I2C_send(0x10, engn_forward, 5);
            }
            else{
                uint8_t datos_derecha[5] = {0x00, 1, 255-diferencia, 1, diferencia};
                I2C_send(0x10, datos_derecha, 5);
            }
        }

        else if (valor_izq > valor_der){
            diferencia = valor_izq - valor_der;
            if(diferencia > 255){
                I2C_send(0x10, engn_left_hard, 5);
            }
            else if(diferencia < 30){
                I2C_send(0x10, engn_forward, 5);
            }
            else{
            uint8_t datos_izquierda[5] = {0x00, 1, diferencia, 1, 255-diferencia};
            I2C_send(0x10, datos_izquierda, 5);
            }

        }

        else{
            I2C_send(0x10, engn_forward, 5);
        }
    }
    I2C_send(0x10, engn_off, 5);
}

void linetrack (){

    uint8_t lectura_sensores;
    uint8_t linetrack_reg = 0x1D;               //direccion de lectura de los sensores del linetrack

    while(!modo_menu){

        I2C_send(0x10, &linetrack_reg, 1);          //nos comunicamos con los sensores para poder leer lo que midan
        I2C_receive(0x10, &lectura_sensores, 1);    //Recibimos la lectura por i2c de los sensores


        if((lectura_sensores & 0x20)){
            I2C_send(0x10, led_on_forward, 3);
            // I2C_send(0x10, engn_on, 5);         //Seguimos en linea recta
            // LCD_send_text_2lines("@LINE TRACK", "@CENTRADO");
        }

        // LCD_send_text_2lines("@Lectura Sensores", "@%b", lectura_sensores);

        if (lectura_sensores > 35 && lectura_sensores < 48)
        {
            I2C_send(0x10, engn_left_hard, 5);
        }
        if (lectura_sensores >= 48)
        {
            I2C_send(0x10, engn_right_hard, 5);
        }
        if (lectura_sensores = 33)
        {
            I2C_send(0x10, engn_forward, 5);
        }


    }
}

void wifi(void){}


void joystick_move(void){

    uint8_t i;

    while(!modo_menu){
        if (flag_B5 && motorjoystick) {
            //estado = 0x01 ;         //estado joystick eje X hacia abajo
            I2C_send(0x10, engn_on, 5);
            for (i = 0 ; i < 5 ; i++){
                I2C_send(0x10, led_on_forward, 3);
                delay_ms(100);
                I2C_send(0x10, led_off, 3);
                delay_ms(100);
            }
            I2C_send(0x10, engn_off, 5);
            flag_B5 = 0;
        }

        if (flag_B4 && motorjoystick) {
            //estado = 0x03;         //estado joystick eje Y hacia abajo
            I2C_send(0x10, engn_back, 5);
            for (i = 0 ; i < 5 ; i++){
                I2C_send(0x10, led_on_back, 3);
                delay_ms(100);
                I2C_send(0x10, led_off, 3);
                delay_ms(100);
            }
            I2C_send(0x10, engn_off, 5);
            flag_B4 = 0;
        }

        if (flag_B2 && motorjoystick) {
            //estado = 0x03;         //estado joystick eje Y hacia abajo
            I2C_send(0x10, engn_on_turn_left, 5);
            for (i = 0 ; i < 3 ; i++){
                I2C_send(0x10, led_on_turn_left, 3);
                delay_ms(500);
                I2C_send(0x10, led_off, 3);
                delay_ms(500);
            }
            I2C_send(0x10, engn_off, 5);
            flag_B2 = 0;
        }

        if (flag_B3 && motorjoystick) {
            //estado = 0x03;         //estado joystick eje Y hacia abajo
            I2C_send(0x10, engn_on_turn_right, 5);
            for (i = 0 ; i < 3 ; i++){
                I2C_send(0x10, led_on_turn_right, 3);
                delay_ms(500);
                I2C_send(0x10, led_off, 3);
                delay_ms(500);
            }
            I2C_send(0x10, engn_off, 5);
            flag_B3 = 0;
        }
    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    __disable_interrupt();

    init_clocks();
    init_timers();
    init_joystick();

    __enable_interrupt();
    I2C_init();
    I2C_display();
    init_ADC();

    // Saludos de cortesia para que arranque el menú
    LCD_send_text("@HOLA");
    delay_ms(2000);
    LCD_send_text("@Somos Manu y Max");
    delay_ms(2000);
    LCD_send_text("@Motoresss");
    delay_ms(2000);
    LCD_send_text("@Preparados");
    delay_ms(500);
    LCD_send_text("@Listos");
    delay_ms(500);
    LCD_send_text("@Ya!");

    uint8_t rd_linetrack;

    while(1){
        uint8_t lectura_sensores;
        uint8_t linetrack_reg = 0x1D;

        I2C_send(0x10, &linetrack_reg, 1);          //nos comunicamos con los sensores para poder leer lo que midan
        I2C_receive(0x10, &lectura_sensores, 1);
        if((lectura_sensores == 0) || (lectura_sensores == 63)){
            I2C_send(0x10, engn_forward, 5);
        }
        else if((lectura_sensores == 0) || (lectura_sensores == 63)){ // izquierda
            I2C_send(0x10, engn_right_soft, 5);
            while(!(lectura_sensores == 12));
        }

        else if((lectura_sensores > 15) && (lectura_sensores < 63 )){ // derecha
            I2C_send(0x10, engn_left_soft, 5);
            while(!(lectura_sensores == 12));
        }
        else if ((lectura_sensores >=7 ) && (lectura_sensores < 12 )) {
            I2C_send(0x10, engn_left_soft, 5);
            while(!(lectura_sensores == 12));

        }
        else {
            // recto
            I2C_send(0x10, engn_forward, 5);
        }


        motorjoystick = 0;
        menu = 0;
        LCD_send_text("@Menu");
        delay_ms(1000);

        while(modo_menu){

            switch (menu_state){

                case 0:

                    LCD_send_text("@1. Modo Joystick");
                    if (flag_menu_down){              // boton UP / NEXT
                        menu_state = 1;
                        flag_menu_down = 0;
                        flag_B5 = 0;
                        delay_ms(150);
                    }

                    if (flag_menu_up){   //back
                        menu_state = 3;
                        flag_menu_up = 0;
                        flag_B4 = 0;
                        delay_ms(150);
                    }

                    if (flag_B3){                //ENTER
                        LCD_send_text_2lines("@1.Modo Joystick", "@ MODO: ACTIVADO");
                        menu = 1;
                        modo_menu = 0;
                        motorjoystick = 1;
                        flag_B1 = 0;
                        flag_B3 = 0;
                        delay_ms(150);
                    }

                break;

                case 1:

                    LCD_send_text("@2. Modo linetrack");
                    if (flag_menu_down){   //next
                        menu_state = 2;
                        flag_menu_down = 0;
                        flag_B5 = 0;
                        delay_ms(150);
                    }

                    if (flag_menu_up){   //back
                        menu_state = 0;
                        flag_menu_up = 0;
                        flag_B4 = 0;
                        delay_ms(150);
                    }

                    if (flag_B3){
                        LCD_send_text_2lines("@2. Modo linetrack", "@MODO: ACTIVADO");
                        menu = 2;
                        modo_menu = 0;
                        flag_B3 = 0;
                        flag_B1 = 0;
                        delay_ms(150);
                    }

                break;

                case 2:

                    LCD_send_text("@3. Modo LDRs");
                    if (flag_menu_down){   //next
                        menu_state = 3;
                        flag_menu_down = 0;
                        flag_B5 = 0;
                        delay_ms(150);
                    }

                    if (flag_menu_up){   //back
                        menu_state = 1;
                        flag_menu_up = 0;
                        flag_B4 = 0;
                        delay_ms(150);
                    }

                    if (flag_B3){
                        LCD_send_text_2lines("@3. Modo LDRs", "@MODO: ACTIVADO");
                        menu = 3;
                        modo_menu = 0;
                        flag_B3 = 0;
                        flag_B1 = 0;
                        delay_ms(150);
                    }

                break;

                case 3:

                    LCD_send_text("@4. Modo WiFi");
                    if (flag_menu_down){   //next
                        menu_state = 0;
                        flag_menu_down = 0;
                        flag_B5 = 0;
                        delay_ms(150);
                    }

                    if (flag_menu_up){   //back
                        menu_state = 2;
                        flag_menu_up = 0;
                        flag_B4 = 0;
                        delay_ms(150);
                    }

                    if (flag_B3){
                        LCD_send_text_2lines("@4. Modo WiFi", "@MODO: ACTIVADO");
                        menu = 4;
                        modo_menu = 0;
                        flag_B3 = 0;
                        flag_B1 = 0;
                        delay_ms(150);
                    }

                break;
            }

        }

        I2C_send(0x10, led_on, 3);      //encendemos los leds. esto nos servira para saber de manera visual si funciona la
                                            //comunicacion I2C con el robot
        if (menu == 1){                  //condicion para entrar en la funcion de mover con el joystick
            joystick_move();
        }

        if (menu == 2){                   //condicion para entrar en la funcion de linetrack
            // linetrack();
            uint8_t lectura_sensores;
            uint8_t linetrack_reg = 0x1D;               //direccion de lectura de los sensores del linetrack

            while(!modo_menu){

                I2C_send(0x10, &linetrack_reg, 1);          //nos comunicamos con los sensores para poder leer lo que midan
                I2C_receive(0x10, &lectura_sensores, 1);    //Recibimos la lectura por i2c de los sensores

                if((lectura_sensores & 0x20)){
                    I2C_send(0x10, led_on_forward, 3);
                    // I2C_send(0x10, engn_on, 5);         //Seguimos en linea recta
                    // LCD_send_text_2lines("@LINE TRACK", "@CENTRADO");
                }
            }
        }

        if (menu == 3){                   //condicion para entrar en la funcion de linetrack
            ldrs();
        }

        if (menu == 4){                   //condicion para entrar en la funcion de linetrack
            wifi();
        }

    }

    return 0;

}

#pragma vector=PORT4_VECTOR
__interrupt void port4_ISR_dirY(void){
     /*Esta interrupcion permite es la direccion Y, sentido arriba y abajo (movimiento y menus)
     * Cuando salte la interrupcion, se activara la banderita que permitira identificar
     * en el resto del código que se ha movido el joystick en esa dirección*/

    if (P4IFG & BIT5 )           //condicion interrupcion sentido arriba
    {
        P4IFG &= ~BIT5;
        flag_B5 = 1;
        if(modo_menu){
            flag_menu_down=1;
        }
    }

    if (P4IFG & BIT6)
    {
        P4IFG &= ~BIT6;
        flag_B4 = 1;
        if(modo_menu){
            flag_menu_up=1;
        }
    }

}

#pragma vector=PORT2_VECTOR
__interrupt void port2_ISR_dirX(void){
 /*Esta interrupcion permite es la direccion X, sentido izquierda del joystick.
 * Cuando salte la interrupcion, se activara la banderita que permitira identificar
 * en el resto del código que se ha movido el joystick en esa dirección*/

    if (P2IFG & BIT5)           //condicion interrupcion joystick direccion izquierda
    {
        P2IFG &= ~BIT5;         //se deshabilita la bandera de interrupcion
        flag_B2 = 1;           //activar la banderilla para notificar en el main que  se ha movido el joystick en sentido izquierda
    }
}

#pragma vector=PORT3_VECTOR
__interrupt void port3_ISR_dirX(void){
    /*Esta interrupcion permite es la direccion X, sentido derecha del joystick y parte del manejo de menus
     * Como el circuito impreso está mal diseñado y no va ese sentido, se ha usado el boton
     * SW3 (P3.5). Cuando salte la interrupcion, se activara la banderita que permitira identificar
     * en el resto del código que se ha presionado el botón .
     *
     * La seleccion de menus se llevará a cabo con el botón SW2 (P3.4). Cuando este se presione,
     * el robot debe detener lo que está haciendo y regresar al menú de selección de modo de trabajo. */

    if (P3IFG & BIT5)           //condicion interrupcion Switch dirección derecha / aceptar modo
    {
        P3IFG &= ~BIT5;         //se deshabilita la bandera de interrupcion
        flag_B3 = 1;            //activar la banderilla para notificar en el main que  se ha pulsado el SW3
    }

    if (P3IFG & BIT4)
    {
        P3IFG &= ~BIT4;        //se deshabilita la bandera de interrupcion
        modo_menu = 1;
        menu_state = 0;
        menu = 0;
        flag_B1 = 1;           //activar la banderilla para notificar en el main que  se ha pulsado el SW2
    }
}
