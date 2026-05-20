/*
* power_supplies.cpp
*
* Created: 22/02/2026 10:13:35
*  Author: Gebruiker
*/
#include <avr/io.h>
#include "power_supplies.h"
#include <util/delay.h>
#include "pinmap.h"
#include "adc.h"
#include "geiger.h"
#include <avr/interrupt.h>

// Target feedback voltage (scaled from 400V divider)
#define HV_TARGET_FB     2.7f    // example: 2.7V at PC5 = 400V output
#define HV_KP            20.0f   // proportional gain (tune)
#define HV_KI            1.0f   // proportional gain (tune)
#define HV_PWM_MAX       102 //Max 10%
#define HV_INT_MAX   200.0f
#define HV_INT_MIN  -200.0f

volatile float hv_voltage = 0.0f;
static float hv_integral = 0.0f;
unsigned long lastHVUpdate = 0;


void power_supplies_init(void)
{
	//Set enable pins to output mode
	PSU_12V_DDR  |= (1 << PSU_12V_PIN);
	PSU_180V_DDR |= (1 << PSU_180V_PIN);

	enable_12V(1);
	_delay_ms(100);
	enable_180V(1);
}

void enable_180V(uint8_t enable){
	if (enable)
	PSU_180V_PORT |=  (1 << PSU_180V_PIN);
	else
	PSU_180V_PORT &= ~(1 << PSU_180V_PIN);
}

void enable_12V(uint8_t enable){
	if (enable)
	PSU_12V_PORT |=  (1 << PSU_12V_PIN);
	else
	PSU_12V_PORT &= ~(1 << PSU_12V_PIN);
}




float hv_supply_update(uint16_t adc_value)
{
	hv_voltage = adc_to_voltage(adc_value);

	float error = HV_TARGET_FB - hv_voltage;

	// integrate error
	hv_integral += error;

	// anti-windup clamp
	if (hv_integral > 200.0f)  hv_integral = 200.0f;
	if (hv_integral < -200.0f) hv_integral = -200.0f;

	float out = (HV_KP * error) + (HV_KI * hv_integral);
	int32_t duty = (int32_t)out;

	if (duty < 0) duty = 0;
	if (duty > HV_PWM_MAX) duty = HV_PWM_MAX;

	OCR1A = (uint16_t)duty;
	return hv_voltage;
}

// ISR(TIMER0_COMPA_vect)
// {
// 	// --- buzzer timing ---
// 	if (buzzer_timer > 0)
// 	{
// 		buzzer_timer--;
// 		if (buzzer_timer == 0)
// 		PORTB &= ~(1 << PWM_BUZZER_PIN);   // turn buzzer OFF
// 	}
// 
// 	// --- 1-second timing ---
// 	// --- 1-second timing ---
// 	t0_overflows++;
// 	if (t0_overflows >= 3906)   // 1 second at 256 µs tick
// 	{
// 		t0_overflows = 0;
// 
// 		// Remove oldest second
// 		cpm -= pulses_last_10s[sec_index];
// 
// 		// Insert new second
// 		pulses_last_10s[sec_index] = current_second_pulses;
// 
// 		// Add new second
// 		cpm += current_second_pulses;
// 
// 		// Advance index
// 		sec_index++;
// 		if (sec_index >= 10)
// 		sec_index = 0;
// 
// 		// Reset for next second
// 		current_second_pulses = 0;
// 	}
// 
// 	uint16_t cpm_scaled = cpm * 6;   // convert 10-second count to CPM
// 	usv_h = cpm_scaled * CPM_TO_USV;
// 	// --- HV update divider ---
// 	static uint8_t div = 0;
// 	if (++div < 10)
// 	return;
// 	div = 0;
// 
// 	hv_supply_update();
// }

