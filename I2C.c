/*
 * I2C.c
 *
 *  Created on: 6 abr. 2026
 *      Author: Max y Manuel
 */

#include <msp430.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "delay.h"

uint8_t *PTxData; // Pointer to TX data
uint8_t TXByteCtr;
uint8_t *PRxData; // Pointer to RX data
uint8_t RXByteCtr;
volatile unsigned int contador_timer = 0;      // Cuenta ticks del reloj
volatile uint8_t seguir = 0;
volatile uint16_t valor_limite = 0;

void I2C_init()
{
    // cambiar codigo cuando tengamos nuestra placa --> cambiar de B0 a B1
    //P4SEL0 |= BIT7 + BIT6;  //* P4.6 SDA i P4.7 SCL com a USCI si fem server USCI B1
    P1SEL0 |= BIT3 | BIT2;  //* P1.2 SDA i P1.3 SCL com a USCI si fem server USCI B0
    UCB0CTLW0 |= UCSWRST; // Aturem el m�dul
    //El configurem com a master, s�ncron i mode i2c, per defecte, est� en single-master mode
    UCB0CTLW0 |= UCMST | UCMODE_3 | UCSSEL_2; // Use SMCLK,
    UCB0BR0 = 160; // fSCL = SMCLK(16MHz)/160 = ~100kHz
    UCB0BR1 = 0;
    UCB0CTLW0 &= ~UCSWRST; // Clear SW reset, resume operation
    UCB0IE |= UCTXIE0 | UCRXIE0; // Habilita les interrupcions a TX i RX
}

void I2C_send(uint8_t addr, uint8_t *buffer, uint8_t n_dades){
    // cambiar codigo cuando tengamos nuestra placa --> cambiar de B0 a B1
    UCB0I2CSA = addr; //Coloquem l�adre�a de slave
    PTxData = buffer; //adre�a del bloc de dades a transmetre
    TXByteCtr = n_dades; //carreguem el n�mero de dades a transmetre;
    UCB0CTLW0 |= UCTR + UCTXSTT; //I2C en mode TX, enviem la condici� de start
    __bis_SR_register(LPM0_bits + GIE); //Entrem a mode LPM0, enable interrupts
    __no_operation(); //Resta en mode LPM0 fins que es trasmetin les dades
    while (UCB0CTLW0 & UCTXSTP); //Ens assegurem que s'ha enviat la condici� de stop
    delay_ms(10);
}

void I2C_receive(uint8_t addr, uint8_t *buffer, uint8_t n_dades){
    // cambiar codigo cuando tengamos nuestra placa --> cambiar de B0 a B1
    PRxData = buffer; //adre�a del buffer on ficarem les dades rebudes
    RXByteCtr = n_dades; //carreguem el n�mero de dades a rebre
    UCB0I2CSA = addr; //Coloquem l�adre�a de slave
    UCB0CTLW0 &= ~UCTR; //I2C en mode Recepci�
    while (UCB0CTLW0 & UCTXSTP); //Ens assegurem que el bus est� en stop
    UCB0CTLW0 |= UCTXSTT; //I2C start condition en recepci�
    __bis_SR_register(LPM0_bits + GIE); //Entrem en mode LPM0, enable interrupts
}

void I2C_display()
{
    P5DIR |= BIT2;
    P5OUT &= ~BIT2;  // Ponemos el pin RST a 0 (Inicia el reinicio)
    delay_ms(10);    // Esperamos 10ms con el chip apagado
    P5OUT |= BIT2;   // Ponemos el pin RST a 1 (Encendemos el chip)
    delay_ms(50);

 // --- SECUENCIA DE INICIALIZACI�N DEL LCD ---
    uint8_t lcd_init_cmd[] = {
        0x00,
        0x39,
        0x14, // Internal OSC frequency
        0x74, // Contraste (bits bajos)
        0x54, // Power/ICON control / Contraste (bits altos)
        0x6F, // Follower control
        0x0C, // Display ON, Cursor OFF
        0x01 // Limpiar pantalla (Clear Display)
    };
    // -------------------------------------------
    // === 2. INICIALIZACI�N POR SOFTWARE (I2C) ===
    I2C_send(0x3E, lcd_init_cmd, sizeof(lcd_init_cmd));

    delay_ms(50); // Le damos tiempo extra para procesar el "Clear Display"

    uint8_t clear_display[] = {0x00, 0x01};
    I2C_send(0x3E, clear_display, 2);
    // ============================================
}

void LCD_send_text(char *string) {
    char buffer_texto[30];
    uint8_t longitud;

    uint8_t clear_display[] = {0x00, 0x01};
    I2C_send(0x3E, clear_display, 2);
    delay_ms(10);

    longitud = sprintf(buffer_texto, string);
    _NOP();
    I2C_send(0x3E, buffer_texto, longitud);
}

void LCD_send_text_2lines(char *line1, char *line2){
    uint8_t clear_display[] = {0x00, 0x01};
    char buffer_texto1[18];
    char buffer_texto2[18];
    uint8_t longitud1, longitud2;

    // Limpiar display
    I2C_send(0x3E, clear_display, 2);
    delay_ms(10);

    // ----- LINEA 1 -----
    uint8_t cursor_line1[] = {0x00, 0x80};
    I2C_send(0x3E, cursor_line1, 2);
    longitud1 = sprintf(buffer_texto1, line1);
    I2C_send(0x3E, line1,longitud1);

    // ----- LINEA 2 -----
    uint8_t cursor_line2[] = {0x00, 0xC0};
    I2C_send(0x3E, cursor_line2, 2);

    longitud2 = sprintf(buffer_texto2, line2);
    I2C_send(0x3E, line2,longitud2);
}


#pragma vector = USCI_B0_VECTOR
__interrupt void ISR_USCI_I2C(void){
    switch(__even_in_range(UCB0IV,12)){
        case USCI_NONE: break; // Vector 0: No interrupts
        case USCI_I2C_UCALIFG: break; // Vector 2: ALIFG
        case USCI_I2C_UCNACKIFG: break; // Vector 4: NACKIFG
        case USCI_I2C_UCSTTIFG: break; // Vector 6: STTIFG
        case USCI_I2C_UCSTPIFG: break; // Vector 8: STPIFG
        case USCI_I2C_UCRXIFG0: // Vector 10: RXIFG
            if (RXByteCtr){
                *PRxData++ = UCB0RXBUF; // Mou la dada rebuda a l�adre�a PRxData
                if (RXByteCtr == 1) // Queda nom�s una?
                UCB0CTLW0 |= UCTXSTP; // Genera I2C stop condition
            }
            else {
            *PRxData = UCB0RXBUF; // Mou la dada rebuda a l�adre�a PRxData
            __bic_SR_register_on_exit(LPM0_bits); // Exit del mode baix consum LPM0, activa la CPU
            }
            RXByteCtr--; // Decrement RX byte counter
        break;
        case USCI_I2C_UCTXIFG0: // Vector 12: TXIFG
            if (TXByteCtr) {
                UCB0TXBUF = *PTxData++; // Carrega el TX buffer amb la dada a enviar
                TXByteCtr--; // Decrementa TX byte counter
            }
            else{
            UCB0CTLW0 |= UCTXSTP; // I2C stop condition
            UCB0IFG &= ~UCTXIFG; // Clear USCI_B1 TX int flag
            __bic_SR_register_on_exit(LPM0_bits); // Exit del mode baix consum LPM0, activa la CPU
            }
        default: break;
    }
}
