
#ifndef POWER_SUPPLIES_H_
#define POWER_SUPPLIES_H_



extern volatile float hv_voltage;

void power_supplies_init(void);
float hv_supply_update(void);
void hv_pi_timer_init(void);
void hv_pwm_init(void);
void enable_12V(uint8_t enable);
void enable_180V(uint8_t enable);



#endif /* POWER_SUPPLIES_H_ */