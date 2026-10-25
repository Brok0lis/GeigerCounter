#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "timer.h"
#include "power_supplies.h"
#include "hv5222.h"
#include "nixie_display.h"
#include "pinmap.h"
#include "geiger.h"
#include "test.h"
#include <avr/interrupt.h>
#include "controller.h"
#include "uart.h"

void initialize_default_IO_state(){
	// --- Power first: everything off ---
	// 400 V converter gate (low before output, so no glitch)
	PWM_400V_PORT &= ~(1 << PWM_400V_PIN);
	PWM_400V_DDR  |=  (1 << PWM_400V_PIN);

	// 180 V supply off
	PSU_180V_PORT &= ~(1 << PSU_180V_PIN);
	PSU_180V_DDR  |=  (1 << PSU_180V_PIN);

	// 12 V supply off
	PSU_12V_PORT &= ~(1 << PSU_12V_PIN);
	PSU_12V_DDR  |=  (1 << PSU_12V_PIN);

	// Charging off (active low -> drive high)
	ENABLE_CHARGE_PORT |= (1 << ENABLE_CHARGE_PIN);   // level = high
	ENABLE_CHARGE_DDR  |= (1 << ENABLE_CHARGE_PIN);   // then make it an output

	// Display driver, all active low -> idle high
	DRIVER_STROBE_PORT &= ~(1 << DRIVER_STROBE_PIN);   // low
	DRIVER_STROBE_DDR  |=  (1 << DRIVER_STROBE_PIN);   // then output

	DRIVER_DATA_PORT |= (1 << DRIVER_DATA_PIN);
	DRIVER_DATA_DDR  |= (1 << DRIVER_DATA_PIN);

	DRIVER_CLOCK_PORT |= (1 << DRIVER_CLOCK_PIN);
	DRIVER_CLOCK_DDR  |= (1 << DRIVER_CLOCK_PIN);

	DRIVER_ENABLE_PORT |= (1 << DRIVER_ENABLE_PIN);   // active low, high = display blanked/disabled
	DRIVER_ENABLE_DDR  |= (1 << DRIVER_ENABLE_PIN);

	// --- Buzzer: silent ---
	PWM_BUZZER_PORT &= ~(1 << PWM_BUZZER_PIN);
	PWM_BUZZER_DDR  |=  (1 << PWM_BUZZER_PIN);

	// --- Digital inputs, no pull-ups ---
	DCF_DATA_DDR   &= ~(1 << DCF_DATA_PIN);
	DCF_DATA_PORT  &= ~(1 << DCF_DATA_PIN);

	PULSE_DETECT_DDR  &= ~(1 << PULSE_DETECT_PIN);    // PD2 (INT0)
	PULSE_DETECT_PORT &= ~(1 << PULSE_DETECT_PIN);

	USB_DETECT_DDR  &= ~(1 << USB_DETECT_PIN);
	USB_DETECT_PORT &= ~(1 << USB_DETECT_PIN);

	// --- ADC inputs, no pull-ups ---
	ADC_400V_DETECT_DDR  &= ~(1 << ADC_400V_DETECT_PIN);
	ADC_400V_PORT        &= ~(1 << ADC_400V_DETECT_PIN);

	ADC_BRIGHTNESS_DDR  &= ~(1 << ADC_BRIGHTNESS_PIN);
	ADC_BRIGHTNESS_PORT &= ~(1 << ADC_BRIGHTNESS_PIN);

	ADC_MODE_DDR  &= ~(1 << ADC_MODE_PIN);
	ADC_MODE_PORT &= ~(1 << ADC_MODE_PIN);



}

uint8_t check_if_charger_plugged(){
	
	return (PINC & (1 << USB_DETECT_PIN)) != 0;
}

void enable_charging(uint8_t enable)
{
	if (enable)
	{
		ENABLE_CHARGE_PORT &= ~(1 << ENABLE_CHARGE_PIN);   // low = charger on
	}
	else
	{
		ENABLE_CHARGE_PORT |= (1 << ENABLE_CHARGE_PIN);    // high = charger off
	}
}

void enter_running(void)
{
	enable_charging(0);              // charger off first
	power_supplies_enable(1);
	adc_init();
	timer_1ms_init();
	geiger_int_init();
	timer1_init();
	timer2_init();
	nixie_clear_all_digits();
	sei();
}

void enter_charging(void)
{
	cli();
	TCCR1A = 0; TCCR1B = 0;          // stop Timer1 (HV + brightness PWM)
	TCCR2A = 0; TCCR2B = 0;          // stop Timer2
	// also stop whichever timer timer_1ms_init() uses, and disable the geiger INT0
	EIMSK &= ~(1 << INT0);

	power_supplies_enable(0);        // high voltage off first
	initialize_default_IO_state();   // all pins to their safe levels
	enable_charging(1);              // charger on last
}


void run(void)
{
	run_state_t state = STATE_NONE;

	initialize_default_IO_state();
	uart_init();
	_delay_ms(500);

	while (1)
	{
		// --- detect charger and switch mode only on a change ---
		run_state_t wanted = check_if_charger_plugged() ? STATE_CHARGING : STATE_RUNNING;

		if (wanted != state)
		{
			state = wanted;
			if (state == STATE_CHARGING) enter_charging();
			else                         enter_running();
		}

		if (state == STATE_CHARGING)
		{
			continue;                // idle (or sleep) while charging
		}

		// --- normal operation ---
		check_1s_flag();
		if (tick_1s_flag)
		{
			radiation_process_1s(current_second_pulses);
			reset_1s_flag();
			current_second_pulses = 0;
		}

		if (tick_1ms_flag)
		{
			tick_1ms_flag = 0;

			static uint8_t adc_div = 0;
			if (++adc_div >= 10) { adc_div = 0; adc_update(); }

			switch (mode_from_adc())
			{
				case MODE_CPM:  display_count(radiation_get_cpm());        break;
				case MODE_USV:  display_usv(get_usv());                    break;
				case MODE_TIME: display_count(geiger_total_count);         break;
			}
			nixie_update_brightness(adc_get_brightness());

			hv_supply_update(adc_get_hv());
		}
	}
}