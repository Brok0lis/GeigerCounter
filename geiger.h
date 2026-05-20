#ifndef GEIGER_H_
#define GEIGER_H_



extern volatile uint32_t geiger_count;
extern volatile uint32_t geiger_total_count;
extern volatile uint8_t buzzer_timer;
static const float CPM_TO_USV = 0.0057f;
extern volatile uint8_t pulses_last_10s[10];
extern volatile uint8_t sec_index;
extern volatile uint16_t cpm;
extern volatile uint8_t current_second_pulses;

extern volatile uint16_t t0_overflows;

extern float usv_h;

void radiation_process_1s(uint16_t current_second_pulses);
void geiger_int_init(void);
float get_usv();

#endif /* GEIGER_H_ */
