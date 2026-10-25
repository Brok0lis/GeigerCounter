#ifndef CONTROLLER_H_
#define CONTROLLER_H_
#include <avr/io.h>

void initialize_default_IO_state();
uint8_t check_if_charger_plugged();
void enable_charging(uint8_t enable);
void run();


#endif 