#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <avr/io.h>

typedef enum { STATE_NONE, STATE_RUNNING, STATE_CHARGING } run_state_t;
void initialize_default_IO_state();
void enter_running(void);
void enter_charging(void);
uint8_t check_if_charger_plugged();
void enable_charging(uint8_t enable);
void run();


#endif 