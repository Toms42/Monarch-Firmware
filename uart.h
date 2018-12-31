/*
 * uart.h
 *
 *  Created on: Dec 22, 2018
 *      Author: Jae Choi
 */

#ifndef UART_H_
#define UART_H_

#include "driverlib.h"

void uart_init_GPIO(uint8_t port, uint16_t pin_tx, uint16_t pin_rx, uint8_t function);

void uart_init();

void uart_putc(char c);
void uart_puts(char *s, uint32_t len);
char uart_getc();


#endif /* UART_H_ */
