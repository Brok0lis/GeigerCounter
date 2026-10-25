#include <avr/io.h>
#include "adc.h"

#include <util/delay.h>

#define MODE_CPM_MAX     90
#define MODE_USV_MIN     240
#define MODE_USV_MAX     405
#define MODE_TIME_MIN    675


static uint8_t adc_turn = 0;   // 0 = brightness, 1 = HV, 2 = mode
static uint16_t adc_brightness_value = 0;
static uint16_t adc_hv_value = 0;
static uint16_t adc_mode_value = 0;


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

static void adc_convert_blocking(void)
{
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
}


void adc_update(void)
{
	// 1. Select the channel for this turn
	uint8_t ch = (adc_turn == 0) ? ADC_CH_BRIGHTNESS :
	(adc_turn == 1) ? ADC_CH_400V :
	ADC_CH_MODE;

	ADMUX = (ADMUX & 0xF0) | (ch & 0x0F);

	// 2. Dummy conversions: flush the charge left from the previous channel
	adc_convert_blocking();
	adc_convert_blocking();

	// 3. Real conversion
	adc_convert_blocking();
	uint16_t result = ADC;

	// 4. Store result for this channel
	if (adc_turn == 0)      adc_brightness_value = result;
	else if (adc_turn == 1) adc_hv_value = result;
	else                    adc_mode_value = result;

	// 5. Next channel on the next call
	if (++adc_turn >= 3)
	adc_turn = 0;
}

uint16_t adc_get_brightness(void)
{
	return adc_brightness_value;
}

uint16_t adc_get_hv(void)
{
	return adc_hv_value;
}

uint16_t adc_get_mode(void)
{
	return adc_mode_value;
}


display_mode_t mode_from_adc()
{
	if (adc_mode_value < MODE_CPM_MAX)
	return MODE_CPM;

	if (adc_mode_value > MODE_USV_MIN && adc_mode_value < MODE_USV_MAX)
	return MODE_USV;

	if (adc_mode_value > MODE_TIME_MIN)
	return MODE_TIME;

	// fallback (should never happen)
	return MODE_CPM;
}
