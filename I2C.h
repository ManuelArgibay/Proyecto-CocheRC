/*
 * I2C.h
 *
 *  Created on: 6 abr. 2026
 *      Author: Max y Manuel
 */

#ifndef I2C_H_
#define I2C_H_


#include <msp430.h>
#include <stdint.h>

void I2C_init();
void I2C_send(uint8_t addr, uint8_t *buffer, uint8_t n_dades);
void I2C_receive(uint8_t addr, uint8_t *buffer, uint8_t n_dades);
void I2C_display();
void LCD_send_text(char *string);
void LCD_send_text_2lines(char *line1, char *line2);
void I2C_send_receive(uint8_t slave_addr, uint8_t *wr_buffer, uint8_t *rd_buffer, uint8_t n_dades);



#endif /* I2C_H_ */
