#define MSP430  (430)
#define ATM328  (328)

#define CPU     (ATM328)

#if CPU == ATM328
#include <avr/io.h>
#define PORT_PIN        3
#define LED_BIT_OUT     (1 << (PORT_PIN))
#define LED_PORT_OUT    (PORTB)
#define delay_cycle     __builtin_avr_delay_cycles

#define KEY_IN_PORT     (PORTC)

#define ONE_HIGH    (14)
#define ONE_LOW     (1)
#define ZERO_HIGH   (0)
#define ZERO_LOW    (7)

#define PIXEL_GAP     (40)

#else
#include <MSP430.h> 
#define LED_BIT_OUT     (1 << 5)
#define LED_PORT_OUT    (P1OUT)
#define delay_cycle     __delay_cycles

#define ZERO_HIGH   (8)
#define ZERO_LOW    (2)
#define ONE_HIGH    (1)
#define ONE_LOW     (0)

#define PIXEL_GAP     (50)


#endif