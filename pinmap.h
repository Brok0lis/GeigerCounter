#ifndef PINMAP_H
#define PINMAP_H

// Display driver pins
#define DRIVER_STROBE_PORT   PORTE
#define DRIVER_STROBE_DDR    DDRE
#define DRIVER_STROBE_PIN    PE0

#define DRIVER_DATA_PORT     PORTE
#define DRIVER_DATA_DDR      DDRE
#define DRIVER_DATA_PIN      PE1

#define DRIVER_CLOCK_PORT    PORTE
#define DRIVER_CLOCK_DDR     DDRE
#define DRIVER_CLOCK_PIN     PE2

#define DRIVER_ENABLE_PORT   PORTB
#define DRIVER_ENABLE_DDR    DDRB
#define DRIVER_ENABLE_PIN    PB2

// DCF77 data pin
#define DCF_DATA_PORT  PORTD
#define DCF_DATA_DDR   DDRD
#define DCF_DATA_PIN   PD6

// Power supply enable pins
#define PSU_12V_PORT  PORTD
#define PSU_12V_DDR   DDRD
#define PSU_12V_PIN   PD5

#define PSU_180V_PORT  PORTD
#define PSU_180V_DDR   DDRD
#define PSU_180V_PIN   PD4

// Geiger tube pulse detect pin
#define PULSE_DETECT_PORT  PORTD
#define PULSE_DETECT_DDR   DDRD
#define PULSE_DETECT_PIN   PD2

// ADC input pins
#define ADC_400V_PORT  PORTC
#define ADC_400V_DETECT_DDR   DDRC
#define ADC_400V_DETECT_PIN   PC5

#define ADC_BRIGHTNESS_PORT  PORTC
#define ADC_BRIGHTNESS_DDR   DDRC
#define ADC_BRIGHTNESS_PIN   PC4

#define ADC_AUDIO_PORT  PORTC
#define ADC_AUDIO_DDR   DDRC
#define ADC_AUDIO_PIN   PC3

//Enable charge pin
#define ENABLE_CHARGE_PORT  PORTC
#define ENABLE_CHARGE_DDR   DDRC
#define ENABLE_CHARGE_PIN   PC1

//USB detect pin
#define USB_DETECT_PORT  PORTC
#define USB_DETECT_DDR   DDRC
#define USB_DETECT_PIN   PC0

//PWM pins
#define PWM_400V_PORT  PORTB
#define PWM_400V_DDR   DDRB
#define PWM_400V_PIN   PB1

#define PWM_BUZZER_PORT  PORTB
#define PWM_BUZZER_DDR   DDRB
#define PWM_BUZZER_PIN   PB0

//ADC channels
#define ADC_CH_BRIGHTNESS    4   // ADC4 = PC4
#define ADC_CH_400V          5   // ADC5 = PC5

#endif
