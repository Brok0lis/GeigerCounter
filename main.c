/*
* Geiger Counter.cpp
*
* Created: 22/02/2026 09:58:24
* Author : Gebruiker
*/
#ifndef F_CPU
#warning	"F_CPU not defined. Define it in project properties."

#elif F_CPU != 8000000
#warning	"Wrong F_CPU frequency!"
#endif

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
volatile uint16_t raw ;
uint16_t temp=0;

int main(void){
	run();
}

	
