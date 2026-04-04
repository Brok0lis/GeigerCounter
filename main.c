/*
* Geiger Counter.cpp
*
* Created: 22/02/2026 09:58:24
* Author : Gebruiker
*/
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "power_supplies.h"
#include "hv5222.h"
#include "nixie_display.h"
#include "pinmap.h"
#include "geiger.h"

static uint16_t last_brightness_update = 0;
#define F_CPU 8000000UL
#include <avr/interrupt.h>

volatile uint16_t raw ;

int main(void)

{
	DDRD |= (1 << PD6);
	_delay_ms(2000);
	power_supplies_init();
	hv5222_init();
	adc_init();
	nixie_pwm_init();
	hv_pwm_init();
	hv_pi_timer_init();
	geiger_int_init();

	sei();
	while(1){
		
		uint16_t diff = (nixie_ms - last_brightness_update);

		//display_usv(usv_h);
		if ((uint16_t)(nixie_ms - last_brightness_update) >= 100)  // every 10 ms
		{		display_voltage(nixie_pwm_duty2);
			last_brightness_update = nixie_ms;
			nixie_update_brightness();
		}


	}
}
