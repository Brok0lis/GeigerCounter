/*
* Geiger Counter.cpp
*
* Created: 22/02/2026 09:58:24
* Author : Gebruiker
*/
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "adc.h"
#include "timer.h"
#include "power_supplies.h"
#include "hv5222.h"
#include "nixie_display.h"
#include "pinmap.h"
#include "geiger.h"


#include <avr/interrupt.h>
volatile uint16_t raw ;
uint16_t temp=0;
int main(void)

{
	
}

void run(){
	
		
		
			DDRD |= (1 << PD6);
			_delay_ms(2000);
			power_supplies_init();
			power_supplies_enable(1);
			hv5222_init();
			adc_init();
			timer_1ms_init();
			geiger_int_init();
			timer1_init();
			timer2_init();
			sei();
			nixie_clear_all_digits();
			while(1){
				check_1s_flag();
				if(tick_1s_flag)
				{
					radiation_process_1s(current_second_pulses);
					reset_1s_flag();
					current_second_pulses=0;
					
				}
				
				
				
				if(tick_1ms_flag)
				{
					adc_update();
					tick_1ms_flag=0;
					float val = adc_to_voltage(adc_get_hv());
					display_mode_t what_to_show =mode_from_adc();
					
					switch (what_to_show)
					{
						case MODE_CPM:

						display_count(radiation_get_cpm());
						
						
						break;

						case MODE_USV:
						
						display_usv(get_usv());

						break;

						case MODE_TIME:
						display_count(geiger_total_count);

						break;
					}

					
					nixie_update_brightness(500);
					//nixie_update_brightness(adc_get_brightness());
					hv_supply_update(adc_get_hv());
					

				}
			
			

		}
	
};
	
