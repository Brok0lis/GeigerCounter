#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "pinmap.h"

// --- Public API ---
void     adc_init(void);
void     adc_update(void);

uint16_t adc_get_brightness(void);
uint16_t adc_get_hv(void);

float    adc_to_voltage(uint16_t raw);

#endif
