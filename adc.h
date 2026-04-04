#ifndef ADC_H
#define ADC_H

#include <stdint.h>
extern volatile uint16_t adc_values[2];
void adc_init(void);
uint16_t adc_read(uint8_t channel);
float adc_to_voltage(uint16_t raw);
uint16_t read_brightness_adc(uint8_t channel);

#endif
