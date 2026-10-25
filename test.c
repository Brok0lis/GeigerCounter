#include "test.h"


// Delays for a run-time value in ms.
// _delay_ms() needs a compile-time constant, so we call it with a
// fixed 1 ms and repeat it `ms` times.
static void delay_ms_var(uint16_t ms)
{
	while (ms--)
	{
		_delay_ms(1);
	}
}


// Configures PD6 as an output and toggles it forever, waiting
// `miliseconds` between each toggle. One full period (high + low)
// is therefore 2 * miliseconds, so the pin toggles at
// 1 / (2 * miliseconds) Hz. Useful for checking F_CPU and delay
// accuracy with a scope or logic analyzer.

void timing_test(uint16_t miliseconds)
{
	DDRD |= (1 << PD6);              // PD6 as output

	while (1)
	{
		delay_ms_var(miliseconds);
		PORTD ^= (1 << PD6);         // toggle
		delay_ms_var(miliseconds);
	}
}
