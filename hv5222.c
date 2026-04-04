#include <avr/io.h>
#include "hv5222.h"
#include "pinmap.h"
#include <util/delay.h>

static uint64_t hv5222_state = 0;

static inline void invert_pin(volatile uint8_t *port, uint8_t bit, uint8_t value)
{
	if (value == 0)
	*port |= (1 << bit);      // inverted: logical 0 ? physical high
	else
	*port &= ~(1 << bit);     // inverted: logical 1 ? physical low
}

void hv5222_init(void) {
	//Set all the output pins to output
	DRIVER_CLOCK_DDR  |= (1 << DRIVER_CLOCK_PIN);
	DRIVER_ENABLE_DDR |= (1 << DRIVER_ENABLE_PIN);
	DRIVER_DATA_DDR  |= (1 << DRIVER_DATA_PIN);
	DRIVER_STROBE_DDR   |= (1 << DRIVER_STROBE_PIN);
	
	invert_pin(&DRIVER_STROBE_PORT, DRIVER_STROBE_PIN, 1);

}



static inline void write_single_bit(uint8_t bit)
{
	invert_pin(&DRIVER_DATA_PORT, DRIVER_DATA_PIN, bit);
	_delay_us(10);
	invert_pin(&DRIVER_CLOCK_PORT, DRIVER_CLOCK_PIN, 1);
	_delay_us(10);
	invert_pin(&DRIVER_CLOCK_PORT, DRIVER_CLOCK_PIN, 0);
	_delay_us(10);
}

void hv5222_write64(uint64_t value) {
	hv5222_state = value;
	hv5222_enable(0);
	for (int i = 63; i >= 0; i--) {
		uint8_t bit = (value >> i) & 1;
		write_single_bit(bit);
	}
}

void hv5222_set_bit(uint8_t index, uint8_t state) {
	if (state)
	hv5222_state |= ((uint64_t)1 << index);
	else
	hv5222_state &= ~((uint64_t)1 << index);

}

uint64_t hv5222_get_state(void) {
	return hv5222_state;
}


void hv5222_enable(uint8_t enable)
{
	if (enable)
	DRIVER_ENABLE_PORT &= ~(1 << DRIVER_ENABLE_PIN);
	else
	DRIVER_ENABLE_PORT |=  (1 << DRIVER_ENABLE_PIN);
	
}


