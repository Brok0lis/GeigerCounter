#ifndef HV5222_H
#define HV5222_H

#include <stdint.h>

void hv5222_init(void);
void hv5222_write64(uint64_t value);

// Optional helpers
void hv5222_set_bit(uint8_t index, uint8_t state);
uint64_t hv5222_get_state(void);
void hv5222_enable(uint8_t enable);

#endif
