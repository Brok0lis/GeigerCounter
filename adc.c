#include <avr/io.h>
#include "adc.h"
#include <util/delay.h>
void adc_init(void)
{
	ADMUX = (1 << REFS0);  // AVcc reference
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

uint16_t adc_read(uint8_t channel)
{
	static uint8_t last_channel = 0xFF;   // invalid at startup

	// 1. Wait for any ongoing conversion to finish
	while (ADCSRA & (1 << ADSC))
	;

	// 2. Only switch ADMUX if channel changed
	if (channel != last_channel)
	{
		ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
		last_channel = channel;


	}

	// 3. Real conversion
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC))
	;

	return ADC;
}




float adc_to_voltage(uint16_t raw)
{
	return (raw / 1023.0f) * 3.0f;
}


uint16_t read_brightness_adc(uint8_t channel)
{
	uint16_t raw = adc_read(channel);   // PC4 = ADC4
	return raw;
}
