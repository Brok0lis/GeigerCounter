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
volatile uint32_t geiger_count = 0;
volatile uint32_t geiger_total_count = 0;  // lifetime pulses
volatile uint8_t buzzer_timer = 0;       // click duration

volatile uint8_t pulses_last_10s[10] = {0};
volatile uint8_t sec_index = 0;
volatile uint16_t cpm = 0;
volatile uint8_t current_second_pulses = 0;

volatile uint16_t t0_overflows = 0;            // for 1-second timing
volatile float usv_h = 0.0f;





void geiger_int_init(void)
{
	// PD2 (INT0) as input
	DDRD &= ~(1 << PULSE_DETECT_PIN);
	//Set buzzer as output
	DDRB |= (1 << PWM_BUZZER_PIN);


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

	buzzer_timer = 3; //0-8
	PORTB |= (1 << PWM_BUZZER_PIN);     // turn buzzer ON
}
