#include <stdio.h>
#include "Disp.h"
#include "Game.h"

#if CPU == ATM328
void SendWS2811(unsigned char* p, unsigned short nbytes)
{
//    volatile unsigned char*   p = &rgb;   // Copy the start address of our data array
    volatile unsigned char val  = *p++;      // Get the current byte value & point to next byte
    volatile unsigned char high = LED_PORT_OUT |  LED_BIT_OUT; // Bitmask for sending HIGH to pin
    volatile unsigned char low  = LED_PORT_OUT & ~LED_BIT_OUT; // Bitmask for sending LOW to pin
    volatile unsigned char tmp  = low;       // Swap variable to adjust duty cycle 
    volatile unsigned char nbits = 8;  // Bit counter for inner loop
//    volatile unsigned short nbytes = 12; // Byte counter for outer loop
    
//    delay_cycle(10000);
//    asm volatile("cli"); // Disable interrupts so that timing is as precise as possible

  // The volatile attribute is used to tell the compiler not to optimize 
  // this section.  We want every instruction to be left as is.
  //
  // Generating an 800KHz signal (1.25us period) implies that we have
  // exactly 20 instructions clocked at 16MHz (0.0625us duration) to 
  // generate either a 1 or a 0---we need to do it within a single 
  // period. 
  // 
  // By choosing 1 clock cycle as our time unit we can keep track of 
  // the signal's phase (T) after each instruction is executed.
  //
  // To generate a value of 1, we need to hold the signal HIGH (maximum)
  // for 0.8us, and then LOW (minimum) for 0.45us.  Since our timing has a
  // resolution of 0.0625us we can only approximate these values. Luckily, 
  // the WS281X chips were designed to accept a +/- 300ns variance in the 
  // duration of the signal.  Thus, if we hold the signal HIGH for 13 
  // cycles (0.8125us), and LOW for 7 cycles (0.4375us), then the variance 
  // is well within the tolerated range.
  //
  // To generate a value of 0, we need to hold the signal HIGH (maximum)
  // for 0.4us, and then LOW (minimum) for 0.85us.  Thus, holding the
  // signal HIGH for 6 cycles (0.375us), and LOW for 14 cycles (0.875us)
  // will maintain the variance within the tolerated range.
  //
  // For a full description of each assembly instruction consult the AVR
  // manual here: http://www.atmel.com/images/doc0856.pdf
    asm volatile(
        // Instruction        CLK     Description                 Phase
    "nextbit:\n\t"         // -    label                       (T =  0) 
        "sbi  %0, %1\n\t"     // 2    signal HIGH                 (T =  2) 
        "sbrc %4, 7\n\t"      // 1-2  if MSB set                  (T =  ?)          
        "mov  %6, %3\n\t"    // 0-1   tmp'll set signal high     (T =  4) 
        "dec  %5\n\t"         // 1    decrease bitcount           (T =  5) 
        "nop\n\t"             // 1    nop (idle 1 clock cycle)    (T =  6)
        "st   %a2, %6\n\t"    // 2    set PORT to tmp             (T =  8)
        "mov  %6, %7\n\t"     // 1    reset tmp to low (default)  (T =  9)
        "breq nextbyte\n\t"   // 1-2  if bitcount ==0 -> nextbyte (T =  ?)                
        "rol  %4\n\t"         // 1    shift MSB leftwards         (T = 11)
        "rjmp .+0\n\t"        // 2    nop nop                     (T = 13)
        "cbi   %0, %1\n\t"    // 2    signal LOW                  (T = 15)
        "rjmp .+0\n\t"        // 2    nop nop                     (T = 17)
        "nop\n\t"             // 1    nop                         (T = 18)
        "rjmp nextbit\n\t"    // 2    bitcount !=0 -> nextbit     (T = 20)
        "nextbyte:\n\t"        // -    label                       -
        "ldi  %5, 8\n\t"      // 1    reset bitcount              (T = 11)
        "ld   %4, %a8+\n\t"   // 2    val = *p++                  (T = 13)
        "cbi   %0, %1\n\t"    // 2    signal LOW                  (T = 15)
        "rjmp .+0\n\t"        // 2    nop nop                     (T = 17)
        "nop\n\t"             // 1    nop                         (T = 18)
        "dec %9\n\t"          // 1    decrease bytecount          (T = 19)
        "brne nextbit\n\t"    // 2    if bytecount !=0 -> nextbit (T = 20)
        ::
        "I" (_SFR_IO_ADDR(LED_PORT_OUT)), // %0
        "I" (PORT_PIN),           // %1
        "e" (&LED_PORT_OUT),      // %a2
        "r" (high),               // %3
        "r" (val),                // %4
        "r" (nbits),              // %5
        "r" (tmp),                // %6
        "r" (low),                // %7
        "e" (p),                  // %a8
        "w" (nbytes)              // %9
    );
//    asm volatile("sei"); //();                          // Enable interrupts
}
#else
void SendWS2811(unsigned char* p, unsigned short nSize)
{
    char i=12;
    unsigned char P1 = LED_PORT_OUT | LED_BIT_OUT;
    unsigned char P0 = LED_PORT_OUT & ~(LED_BIT_OUT);
    unsigned char H_0 = ZERO_HIGH;
    unsigned char L_0 = ZERO_LOW;
    unsigned char H_1 = ONE_HIGH;
    unsigned char L_1 = ONE_LOW;
    while(i--)
    {
        LED_PORT_OUT = P1;
        if(nV & 0x800000) // 1
        {
            delay_cycle(H_1);  // 1.0 us, ONE_HIGH.
            LED_PORT_OUT = P0;
            delay_cycle(L_1);  // 0.25 us, ONE_LOW
        }
        else  // 0
        {
            delay_cycle(H_0);  // 0.25 us, ZERO_HIGH,
            LED_PORT_OUT = P0;
            delay_cycle(L_0); // 1.0 us, ZERO_LOW,
        }
        nV <<= 1;
        LED_PORT_OUT = P1;
        if(nV & 0x800000) // 1
        {
            delay_cycle(H_1);  // 1.0 us, ONE_HIGH.
            LED_PORT_OUT = P0;
            delay_cycle(L_1);  // 0.25 us, ONE_LOW
        }
        else  // 0
        {
            delay_cycle(H_0);  // 0.25 us, ZERO_HIGH,
            LED_PORT_OUT = P0;
            delay_cycle(L_0); // 1.0 us, ZERO_LOW,
        }
        nV <<= 1;
    }
    delay_cycle(PIXEL_GAP);
}
#endif
void Disp_Init()
{
    return;
}

/**
Show LED status.
Running time : 3.8ms ~ 7.0 ms (for WS2811)
*/
const unsigned long gaColorPal[NUM_COLOR]={
    0x000000,   // BLACK
    0x000F00,   // RED
    0x00000F,   // GREEN
    0x0F0000,   // BLUE
    0x0F0F0F,   // WHITE
    0xFFFFFF,   // DUMMY
};
const unsigned char gaChPal[NUM_COLOR]={
    '.', 'r', 'g', 'b', 'W', 'D'
};

void Disp_Refresh(unsigned char* anPixel, int nNumPixel)
{
    int i;
    unsigned char anWS2811[(60*3)+2];
    unsigned char* pCurPixel = anWS2811;
    unsigned char nColor;
    for(i=0; i< nNumPixel; i++)
    {
        nColor = anPixel[i];
        if(nColor >= NUM_COLOR) nColor = DUMMY;
        *((unsigned long*)pCurPixel) = gaColorPal[nColor];
        putchar(gaChPal[nColor]);
        pCurPixel += 3;
    }
    putchar('\n');
    
    SendWS2811(anWS2811, NUM_LED*3);
    
}

