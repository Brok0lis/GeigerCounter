#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#ifndef UART_H_
#define UART_H_


#define BAUD 9600UL
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_put_uint(uint16_t value);


#endif /* UART_H_ */