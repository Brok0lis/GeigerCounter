#include "uart.h"

void uart_init(void)
{
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	UCSR0B = (1 << TXEN0);                      // enable transmitter
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);     // 8 data bits, no parity, 1 stop bit
}

void uart_putc(char c)
{
	while (!(UCSR0A & (1 << UDRE0)));           // wait until the data register is empty
	UDR0 = c;
}

void uart_puts(const char *s)
{
	while (*s)
	uart_putc(*s++);
}

void uart_put_uint(uint16_t value)
{
	char buf[6];
	utoa(value, buf, 10);
	uart_puts(buf);
}