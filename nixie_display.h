#ifndef NIXIE_DISPLAY_H
#define NIXIE_DISPLAY_H

#include <stdint.h>

extern volatile uint16_t nixie_pwm_duty,duty;
extern volatile uint16_t nixie_ms;
extern uint16_t max_adc_read;
extern volatile uint8_t nixie_pwm_counter;  // 0–100
extern volatile  uint8_t pwm_override;

// -----------------------------
// Symbol tube definitions
// -----------------------------
typedef enum {
	SYM_BLANK = -1,
	SYM_u,
	SYM_P,
	SYM_MINUS,
	SYM_PLUS,
	SYM_m,
	SYM_M,
	SYM_k,
	SYM_PI,
	SYM_PERCENT,
	SYM_n,
	SYM_COUNT
} nixie_symbol_t;

// -----------------------------
// Unit tube definitions
// -----------------------------
typedef enum {
	UNIT_BLANK = -1,
	UNIT_W,
	UNIT_F,
	UNIT_HZ,
	UNIT_H,
	UNIT_V,
	UNIT_S,
	UNIT_OHM,
	UNIT_A,
	UNIT_COUNT
} nixie_unit_t;

// -----------------------------
// Scale types for formatting
// -----------------------------
typedef enum {
	SCALE_MV,     // millivolt
	SCALE_V,      // volt
	SCALE_USV,    // micro-sievert per hour
	SCALE_MSV     // milli-sievert per hour
} scale_t;

// -----------------------------
// Digit formatting structure
// -----------------------------
typedef struct {
	uint8_t d0, d1, d2;
	uint8_t dp0, dp1, dp2;
} nixie_digits_t;

// -----------------------------
// Public API
// -----------------------------
void nixie_update(void);
void nixie_set_digit(uint8_t tube, uint8_t digit, uint8_t dot);
void nixie_set_symbol(nixie_symbol_t sym);
void nixie_set_unit(nixie_unit_t unit);
void nixie_set_blank(uint8_t tube);
void nixie_clear_all_digits(void);
void nixie_display_number(uint16_t value, uint8_t dot0, uint8_t dot1, uint8_t dot2);
void format_3digits_right_aligned(uint32_t value, uint8_t *d0, uint8_t *d1, uint8_t *d2, uint8_t *dp0, uint8_t *dp1, uint8_t *dp2);
void nixie_pwm_init();
void display_count(uint16_t count);
void display_voltage(float v);
void display_usv(float usv);     // NEW: µSv/h display
void nixie_update_brightness(uint16_t adc_value);
#endif
