#include <stdio.h>
#include <avr/io.h>

#define F_CPU 16000000UL

#define BAUDRATE 38400
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)


//=========================================================================//
//     USART의 송신과 수신을 각각 printf, scanf로 사용 처리
//=========================================================================//


void UART_putchar(char c, FILE *stream);
char UART_getchar(FILE *stream);

FILE UartOut = FDEV_SETUP_STREAM(UART_putchar, NULL,_FDEV_SETUP_WRITE);
FILE UartIn = FDEV_SETUP_STREAM(NULL, UART_getchar, _FDEV_SETUP_READ);

void UART_putchar(char c, FILE *stream) 
{
    if (c == '\n') UART_putchar('\r', stream);  // Make \n to \r\n.
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;
}

char UART_getchar(FILE *stream) 
{
    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
    return UDR0;
}

void UART_Init()
{
    UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));

	stdout = &UartOut;	// Use UART as Stdout
	stdin = &UartIn; // 
    
    printf("Arduino Started\n");
}




