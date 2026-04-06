#include <avr/io.h>
#include "adc.h"

#include <util/delay.h>

static uint8_t adc_turn = 0;   // 0 = brightness, 1 = HV
static uint16_t adc_brightness_value = 0;
static uint16_t adc_hv_value = 0;


void adc_init(void)
{
	ADMUX = (1 << REFS0) | ADC_CH_BRIGHTNESS;
	ADCSRA = (1 << ADEN) |
	(1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128
}

float adc_to_voltage(uint16_t raw)
{
	return (raw / 1023.0f) * 3.0f;
}



void adc_update(void)
{
	// 1. Wait for any ongoing conversion
	while (ADCSRA & (1 << ADSC))
	;

	// 2. Start conversion on the channel already selected
	ADCSRA |= (1 << ADSC);

	// 3. Wait for conversion to finish
	while (ADCSRA & (1 << ADSC))
	;

	uint16_t result = ADC;

	// 4. Store result based on turn
	if (adc_turn == 0)
	adc_brightness_value = result;
	else
	adc_hv_value = result;

	// 5. Switch ADMUX for next call (settling happens between calls)
	adc_turn ^= 1;
	uint8_t next = (adc_turn == 0) ? ADC_CH_BRIGHTNESS : ADC_CH_400V;
	ADMUX = (ADMUX & 0xF0) | (next & 0x0F);

}
uint16_t adc_get_brightness(void)
{
	return adc_brightness_value;
}

uint16_t adc_get_hv(void)
{
	return adc_hv_value;
}