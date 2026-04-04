/*
 * geiger.h
 *
 * Created: 20/03/2026 13:28:46
 *  Author: Gebruiker
 */ 


#ifndef GEIGER_H_
#define GEIGER_H_
#define CPM_TO_USV 0.0057f   // SBM-20 / STS-5
extern volatile uint32_t geiger_count;
extern volatile uint32_t geiger_total_count;  // lifetime pulses
extern volatile uint8_t buzzer_timer;
volatile uint8_t pulses_last_10s[10];
volatile uint8_t sec_index;
volatile uint16_t cpm;
volatile uint8_t current_second_pulses;
extern volatile uint16_t t0_overflows;            // for 1-second timing
volatile float usv_h;

void geiger_int_init(void);

#endif /* GEIGER_H_ */