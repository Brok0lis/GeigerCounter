#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

extern volatile uint8_t tick_1ms_flag;

void timer_1ms_init(void);
void timer1_init();
void hv_pwm_set(uint8_t value);
void brightness_pwm_set(uint8_t value);
void timer2_init(void);
void buzzer_pulse_1ms(void);
void test_toggle_pd6(void);
#endif
