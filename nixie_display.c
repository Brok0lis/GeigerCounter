#include "nixie_display.h"
#include <util/delay.h>
#include "hv5222.h"
#include "adc.h"
#include <stdlib.h>
#include "pinmap.h"
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#define DOT_BIT 10
#define BITS_PER_TUBE 11
#define ADC_NOISE_MARGIN 50
volatile uint16_t nixie_pwm_duty = 20;   // ~20% of 255
volatile uint16_t duty = 0;   // ~20% of 255
uint16_t max_adc_read = 0;
volatile uint8_t pwm_override = 0;

volatile uint8_t nixie_pwm_counter = 0;  // 0–100

static const uint8_t digit_to_bit[10] = {
	0,  // digit 0 ? bit 0
	9,  // digit 1 ? bit 9
	8,  // digit 2 ? bit 8
	7,  // digit 3 ? bit 7
	6,  // digit 4 ? bit 6
	5,  // digit 5 ? bit 5
	4,  // digit 6 ? bit 4
	3,  // digit 7 ? bit 3
	2,  // digit 8 ? bit 2
	1   // digit 9 ? bit 1
};
static const uint8_t symbol_to_bit[SYM_COUNT] = {
	0,  // SYM_u
	1,  // SYM_P
	2,  // SYM_MINUS
	3,  // SYM_PLUS
	4,  // SYM_m
	5,  // SYM_M
	6,  // SYM_k
	7,  // SYM_PI
	8,  // SYM_PERCENT
	9   // SYM_n
};
static const uint8_t unit_to_bit[UNIT_COUNT] = {
	0,  // UNIT_W
	1,  // UNIT_F
	2,  // UNIT_HZ
	3,  // UNIT_H
	4,  // UNIT_V
	5,  // UNIT_S
	6,  // UNIT_OHM
	7   // UNIT_A
};

static float scale_value(float v, scale_t s)
{
	switch (s)
	{
		case SCALE_MV:   return v * 1000.0f;   // V ? mV
		case SCALE_MSV:  return v / 1000.0f;   // µSv ? mSv
		default:         return v;             // V or µSv
	}
}

static scale_t choose_voltage_scale(float v)
{
	return (v < 1.0f) ? SCALE_MV : SCALE_V;
}
static scale_t choose_radiation_scale(float usv)
{
	return (usv < 1000.0f) ? SCALE_USV : SCALE_MSV;
}


static void format_3digits(float v, scale_t s,
uint8_t *d0, uint8_t *d1, uint8_t *d2,
uint8_t *dp0, uint8_t *dp1, uint8_t *dp2)
{
	// Clear decimal points
	*dp0 = 0;
	*dp1 = 0;
	*dp2 = 0;

	// ----------------------------------------------------
	// SCALE_MV (millivolt) — integer, no decimal point
	// ----------------------------------------------------
	if (s == SCALE_MV)
	{
		uint16_t x = (uint16_t)v;   // already scaled to mV

		*d0 = (x / 100) % 10;
		*d1 = (x / 10) % 10;
		*d2 = x % 10;

		// Leading zero blanking
		if (*d0 == 0) {
			*d0 = 10;   // blank
			if (*d1 == 0)
			*d1 = 10;   // blank second digit too
		}
		return;
	}

	// ----------------------------------------------------
	// All other scales (V, µSv/h, mSv/h)
	// Use the same decimal placement rules
	// ----------------------------------------------------

	if (v < 10.0f)
	{
		// X.XX  (decimal on tube 1)
		uint16_t x = (uint16_t)(v * 100);

		*d0 = (x / 100) % 10;
		*d1 = (x / 10) % 10;
		*d2 = x % 10;

		*dp1 = 1;
	}
	else if (v < 100.0f)
	{
		// XX.X  (decimal on tube 2)
		uint16_t x = (uint16_t)(v * 10);

		*d0 = (x / 100) % 10;
		*d1 = (x / 10) % 10;
		*d2 = x % 10;

		*dp2 = 1;
	}
	else
	{
		// XXX (no decimal)
		uint16_t x = (uint16_t)v;

		*d0 = (x / 100) % 10;
		*d1 = (x / 10) % 10;
		*d2 = x % 10;
	}
}


void nixie_update(void)
{	
	hv5222_enable(0);
	hv5222_write64(hv5222_get_state());
	if(hv5222_get_enabled()){
		hv5222_enable(1);
	}
	//maybe here check if it has to be on?

}


void nixie_set_digit(uint8_t tube, uint8_t digit, uint8_t dot)
{
	if (tube > 4 || digit > 9)
	return;

	uint8_t base = tube * BITS_PER_TUBE;

	// Clear all bits for this tube
	for (uint8_t i = 0; i < BITS_PER_TUBE; i++)
	hv5222_set_bit(base + i, 0);

	// Set digit cathode
	hv5222_set_bit(base + digit_to_bit[digit], 1);

	// Set dot if requested
	if (dot)
	hv5222_set_bit(base + DOT_BIT, 1);
}

void nixie_set_symbol(nixie_symbol_t sym)
{
	uint8_t base = 3 * BITS_PER_TUBE;  // tube 3 = symbol tube

	// Clear all symbol bits
	for (uint8_t i = 0; i < 10; i++)
	hv5222_set_bit(base + i, 0);

	// Blank? Then we're done
	if (sym == SYM_BLANK)
	return;

	if (sym >= 0 && sym < SYM_COUNT)
	hv5222_set_bit(base + symbol_to_bit[sym], 1);
}

void nixie_set_unit(nixie_unit_t unit)
{
	uint8_t base = 4 * BITS_PER_TUBE-1;  // tube 4 = unit tube. Normally 11 bits per tube, but unit tube has 10 instead of 11.

	// Clear all unit bits
	for (uint8_t i = 0; i < 8; i++)
	hv5222_set_bit(base + i, 0);

	// Blank? Then we're done
	if (unit == UNIT_BLANK)
	return;

	if (unit >= 0 && unit < UNIT_COUNT)
	hv5222_set_bit(base + unit_to_bit[unit], 1);
}





void nixie_set_blank(uint8_t tube)
{
	uint8_t base = tube * BITS_PER_TUBE;

	for (uint8_t i = 0; i < BITS_PER_TUBE; i++)
	hv5222_set_bit(base + i, 0);
}


void nixie_display_number(uint16_t value, uint8_t dot0, uint8_t dot1, uint8_t dot2)
{
	if (value > 999)
	value = 999;

	uint8_t hundreds = value / 100;
	uint8_t tens     = (value / 10) % 10;
	uint8_t ones     = value % 10;

	// Leading zero blanking
	if (value < 100)
	hundreds = 255;   // special "blank" value

	if (value < 10)
	tens = 255;       // blank

	// Tube 0
	if (hundreds == 255)
	nixie_set_blank(0);
	else
	nixie_set_digit(0, hundreds, dot0);

	// Tube 1
	if (tens == 255)
	nixie_set_blank(1);
	else
	nixie_set_digit(1, tens, dot1);

	// Tube 2 (never blank)
	nixie_set_digit(2, ones, dot2);

	nixie_update();
}



void nixie_clear_all_digits(void)
{
	for (uint8_t tube = 0; tube < 3; tube++)   // digit tubes 0,1,2
	{
		uint8_t base = tube * BITS_PER_TUBE;

		for (uint8_t i = 0; i < BITS_PER_TUBE; i++)
		hv5222_set_bit(base + i, 0);
	}
}

void display_voltage(float v)
{
	scale_t s = choose_voltage_scale(v);   // SCALE_MV or SCALE_V
	float scaled = scale_value(v, s);

	uint8_t d0, d1, d2;
	uint8_t dp0, dp1, dp2;

	format_3digits(scaled, s, &d0, &d1, &d2, &dp0, &dp1, &dp2);

	nixie_set_digit(0, d0, dp0);
	nixie_set_digit(1, d1, dp1);
	nixie_set_digit(2, d2, dp2);

	// Symbol tube
	if (s == SCALE_MV)
	nixie_set_symbol(SYM_m);
	else
	nixie_set_symbol(SYM_BLANK);
	// Unit tube always shows V
	nixie_set_unit(UNIT_V);
	nixie_update();
}

void display_usv(float usv)
{
	scale_t s = choose_radiation_scale(usv);
	float scaled = scale_value(usv, s);

	uint8_t d0, d1, d2;
	uint8_t dp0, dp1, dp2;

	format_3digits(scaled, s, &d0, &d1, &d2, &dp0, &dp1, &dp2);

	nixie_set_digit(0, d0, dp0);
	nixie_set_digit(1, d1, dp1);
	nixie_set_digit(2, d2, dp2);

	nixie_set_symbol(s == SCALE_USV ? SYM_u : SYM_m);
	nixie_set_unit(UNIT_S);   // or UNIT_SV_H if you add it

	nixie_update();
}

void display_count(uint16_t count)
{
	scale_t s = SCALE_V;

	uint8_t d0, d1, d2;
	uint8_t dp0, dp1, dp2;

	format_3digits_right_aligned(count,  &d0, &d1, &d2, &dp0, &dp1, &dp2);

	nixie_set_digit(0, d0, dp0);
	nixie_set_digit(1, d1, dp1);
	nixie_set_digit(2, d2, dp2);

	nixie_set_symbol(SYM_BLANK);
	nixie_set_unit(UNIT_BLANK);   

	nixie_update();
}




void nixie_update_brightness(uint16_t adc_value)
{
	#define AVG_COUNT 20
	static uint16_t samples[AVG_COUNT] = {0};
	static uint32_t sum = 0;
	static uint8_t index = 0;

	// rolling average
	sum -= samples[index];
	samples[index] = adc_value;
	sum += adc_value;

	index++;
	if (index >= AVG_COUNT)
	index = 0;

	uint16_t v = sum / AVG_COUNT;   // filtered ADC

	// Realistic range based on divider (0–256)
	const uint16_t MIN_ADC = 40;
	const uint16_t MAX_ADC = 240;

	if (v < MIN_ADC) v = MIN_ADC;
	if (v > MAX_ADC) v = MAX_ADC;

	uint16_t span = MAX_ADC - MIN_ADC;
	uint16_t pos  = v - MIN_ADC;

	// Scale to 0–100%
	uint8_t duty = (pos * 100UL) / span;

	// Optional clamp (minimum glow)
	if (duty < 5)   duty = 0;
	if (duty > 100) duty = 100;

	nixie_pwm_duty = duty;

	// --- SCALE TO 8-BIT PWM (0–255) ---
	// Use 32-bit math to avoid overflow
	uint8_t pwm_value = (uint32_t)duty * 255u / 100u;

	// Write to Timer1 OC1B (PB2)
	brightness_pwm_set(pwm_value);
}

void format_3digits_right_aligned(uint32_t value,
uint8_t *d0, uint8_t *d1, uint8_t *d2,
uint8_t *dp0, uint8_t *dp1, uint8_t *dp2)
{
	// No decimal points for total count
	*dp0 = 0;
	*dp1 = 0;
	*dp2 = 0;

	// Extract digits
	uint8_t hundreds = (value / 100) % 10;
	uint8_t tens     = (value / 10)  % 10;
	uint8_t ones     =  value        % 10;

	// Right?align logic:
	// If value < 10:   "  " + ones
	// If value < 100:  " " + tens + ones
	// Else:            hundreds + tens + ones

	if (value < 10) {
		*d0 = 10;   // blank
		*d1 = 10;   // blank
		*d2 = ones;
	}
	else if (value < 100) {
		*d0 = 10;   // blank
		*d1 = tens;
		*d2 = ones;
	}
	else {
		*d0 = hundreds;
		*d1 = tens;
		*d2 = ones;
	}
}

