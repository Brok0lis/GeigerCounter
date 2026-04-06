/*
* timer.c
* High?speed 10 µs tick + 1 ms tick + Nixie PWM
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"
#include "pinmap.h"
#include "nixie_display.h"

// --- Global flags and variables ---
volatile uint8_t tick_1ms_flag = 0;        // main loop timing flag


// ------------------------------------------------------------
// Initialize Timer0 for 10 µs tick (100 kHz ISR)
// ------------------------------------------------------------
void timer_1ms_init(void)
{
	// CTC mode
	TCCR0A = (1 << WGM01);

	// Prescaler = 64 ? 8 MHz / 64 = 125 kHz ? 8 µs per tick
	TCCR0B = (1 << CS01) | (1 << CS00);

	// 1 ms / 8 µs = 125 ticks ? OCR0A = 124
	OCR0A = 124;

	// Enable compare match interrupt
	TIMSK0 = (1 << OCIE0A);
}

void timer1_init(void)
{
	// OC1A (PB1) and OC1B (PB2) as outputs
	DDRB |= (1 << PB1) | (1 << PB2);

	// Fast PWM 8-bit (Mode 5)
	// WGM10 = 1, WGM11 = 0  ? TCCR1A
	// WGM12 = 1, WGM13 = 0  ? TCCR1B
	TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B = (1 << WGM12) | (1 << CS11);  // prescaler = 8

	// Start both channels at 0% duty
	OCR1A = 0;   // HV PWM
	OCR1B = 0;   // brightness PWM
}

void hv_pwm_set(uint8_t value) {
	OCR1A = value;
}

void brightness_pwm_set(uint8_t value) {
	OCR1B = value;
}

void timer2_init(void)
{
	// CTC mode
	TCCR2A = (1 << WGM21);

	// prescaler = 64
	TCCR2B = (1 << CS22);

	// 1ms at 8MHz
	OCR2A = 125;

	// interrupt disabled initially
	TIMSK2 &= ~(1 << OCIE2A);
}

void buzzer_pulse_1ms(void)
{
	PORTB |= (1 << PWM_BUZZER_PIN);   // ON

	TCNT2 = 0;                        // reset timer
	TIFR2 |= (1 << OCF2A);            // clear compare flag
	TIMSK2 |= (1 << OCIE2A);          // enable interrupt
}



void test_toggle_pd6(void)
{
	PIND = (1 << PD6);   // hardware toggle
}

// ------------------------------------------------------------
// Timer0 ISR @ 10 µs
// - Generates 1 ms tick
// - Runs Nixie PWM (1 kHz, 0–100 duty)
// ------------------------------------------------------------
ISR(TIMER0_COMPA_vect)
{
	// --- 1) Generate 1 ms tick ---
	tick_1ms_flag = 1;
}
ISR(TIMER2_COMPA_vect)
{
	PORTB &= ~(1 << PWM_BUZZER_PIN);  // OFF
	TIMSK2 &= ~(1 << OCIE2A);         // disable interrupt
}
