#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "pinmap.h"

typedef enum {
	MODE_CPM = 0,
	MODE_USV = 1,
	MODE_TIME = 2
} display_mode_t;



// --- Public API ---
void     adc_init(void);
void     adc_update(void);

uint16_t adc_get_brightness(void);
uint16_t adc_get_hv(void);

float    adc_to_voltage(uint16_t raw);
display_mode_t mode_from_adc();

#endif
