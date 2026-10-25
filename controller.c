#include "controller.h"
#include "pinmap.h"
#include "test.h"

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
DRIVER_STROBE_PORT |= (1 << DRIVER_STROBE_PIN);
DRIVER_STROBE_DDR  |= (1 << DRIVER_STROBE_PIN);

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

void run(){
	initialize_default_IO_state();
	_delay_ms(500);
	if(check_if_charger_plugged()){
		enable_charging(1);
	}
}