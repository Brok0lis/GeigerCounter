/*
 * geiger.c
 *
 * Created: 20/03/2026 13:29:16
 *  Author: Gebruiker
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pinmap.h"
#include "geiger.h"
#include "timer.h"
volatile uint32_t geiger_count = 0;
volatile uint32_t geiger_total_count = 0;  // lifetime pulses
volatile uint8_t buzzer_timer = 0;       // click duration

volatile uint8_t pulses_last_10s[10] = {0};
volatile uint8_t sec_index = 0;
volatile uint16_t cpm = 0;
volatile uint8_t current_second_pulses = 0;
float usv_h = 0.0f;
static uint16_t cpm_sum = 0;   // running sum of last 10 seconds




void geiger_int_init(void)
{
	// PD2 (INT0) as input
	DDRD &= ~(1 << PULSE_DETECT_PIN);
	//Set buzzer as output
	DDRB |= (1 << PWM_BUZZER_PIN);
	PORTB &= ~(1 << PWM_BUZZER_PIN);


	// Trigger on FALLING edge: ISC01 = 1, ISC00 = 0
	EICRA |=  (1 << ISC01);
	EICRA &= ~(1 << ISC00);

	// Enable INT0 interrupt
	EIMSK |= (1 << INT0);
}

ISR(INT0_vect)
{
	geiger_total_count++;
	current_second_pulses++;   // count pulse for CPM
	buzzer_pulse_1ms();
	//PORTB |= (1 << PWM_BUZZER_PIN);     // turn buzzer ON
}
void radiation_process_1s(uint16_t current_second_pulses)
{
	// --- static state for 10-second rolling window ---
	#define WINDOW_SECONDS 10


	// --- remove oldest second from running sum ---
	cpm_sum -= pulses_last_10s[sec_index];

	// --- insert new second ---
	pulses_last_10s[sec_index] = current_second_pulses;

	// --- add new second to running sum ---
	cpm_sum += current_second_pulses;

	// --- advance circular index ---
	sec_index++;
	if (sec_index >= WINDOW_SECONDS)
	sec_index = 0;

}

float get_usv()
{
		// --- convert 10-second sum to CPM ---
		uint16_t cpm_scaled = cpm_sum * 6;   // 10s × 6 = 60s

		// --- convert CPM to µSv/h ---
		// Example: SBM-20 ? 151 CPM per µSv/h

		usv_h = cpm_scaled * CPM_TO_USV;

		return usv_h;
}