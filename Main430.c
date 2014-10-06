#include <msp430.h>

#include "Game.h"
#include "Disp.h"


/**
Get key without pending.
*/
unsigned int GetKey()
{
    int nRet = 0;
#if 1 //def _WIN32
    nRet = 0; //rand() % 10;
    if(nRet == 0) nRet = KEY_L;
    else if(nRet == 2) nRet = KEY_R;
    else nRet = 0;
#else
    if(UCSR0A & (1<<RXC0))
    {
        char in;
        in = UDR0;
        if(in == 'z') nRet |= KEY_L;
        else if(in == '/') nRet |= KEY_R;
    }
#endif
    return nRet;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
    P1OUT ^= BIT7;                      // set timer status flag
}

int main()
{
    unsigned int nDspTO =  1;
    unsigned int nTaskTO = 1;
    unsigned int nTO;
    unsigned int nKey;
    GameInfo stGI;
    Handler afHandle[NUM_STATE] = {
            IncScore_Task,
            BlinkScore_Task,
            WaitGame_Task,
            RunGame_Task
        };

    // Stop watchdog timer to prevent time out reset

    WDTCTL = WDTPW | WDTHOLD;   // disable WDT
	BCSCTL1 = CALBC1_1MHZ;   // Setup clock.
	DCOCTL = CALDCO_1MHZ;    // Setup clock.

    P1OUT = 0x00;
    P1DIR |= LED_OUT | BIT7;

    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
    CCR0 =  20000;
/*
    __enable_interrupt();
    while(1)
    {
        __disable_interrupt();
        SendPixel(0xFF0055);
//        Game_RefreshDsp(&stGI);
        __enable_interrupt();
        __delay_cycles(500);
    }
*/
    Game_Init(&stGI);
    Disp_Init();

    while(1)
    {
        __delay_cycles(500);

        nKey = GetKey();

        // Handle Key event.
        if(nKey)
        {
            nTO = afHandle[gnState](&stGI, EVT_KEY, nKey);
            if(nTO) nTaskTO = nTO;
        }

        // Handle Tick event.
        if(nTaskTO > 0)
        {
            if(--nTaskTO == 0)
            {
                nTO = afHandle[gnState](&stGI, EVT_TIMER, 0);
                if(nTO) nTaskTO = nTO;
            }
        }

        if(nDspTO > 0)
        {
            if(--nDspTO == 0)
            {
                nDspTO = Game_RefreshDsp(&stGI);
            }
        }
    }
}

