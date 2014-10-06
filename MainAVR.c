
#include "abstract.h"
#include "Game.h"
#include "Disp.h"

#define BAUDRATE 38400
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

unsigned char GetKeyVal();


/**
Get key without pending.
*/
unsigned char GetKeyVal()
{
    unsigned char nRet = 0;
#if 1 //def _WIN32
    return (~PINC) & (KEY_L | KEY_R);
#else
//    while(0 == (UCSR0A & (1<<RXC0)));

    if(UCSR0A & (1<<RXC0))
    {
        char in;
        in = UDR0;
        if(in == '/') nRet |= KEY_L;
        else if(in == 'z') nRet |= KEY_R;
    }
#endif
    return nRet;
}

int main(int argc, char* argv[])
{
    unsigned int nDspTO =  1;
    unsigned int nTaskTO = 1;
    unsigned int nTO;
    unsigned int nKey;
    unsigned long nColor = 0;
    GameInfo stGI;
    
    Handler afHandle[NUM_STATE] = {
            IncScore_Task,
            BlinkScore_Task,
            WaitGame_Task,
            RunGame_Task
        };

    LED_PORT_OUT = 0x00;
    DDRB |= LED_BIT_OUT;

    MCUCR &= ~PUD;
    KEY_IN_PORT = (KEY_L | KEY_R);  // High for pullup.
    DDRC &= ~(KEY_L | KEY_R);

    Game_Init(&stGI);
    Disp_Init();
    UART_Init();

    while(1)
    {
        delay_cycle(20000);

        nKey = GetKeyVal();

        // Handle Key event.
//        printf("Key :%X", nKey);
        if(nKey)
        {
            printf("Key :%X", nKey);
            nTO = afHandle[gnState](&stGI, EVT_KEY, nKey);
            if(nTO) nTaskTO = nTO;
        }
        else
        {
            if(nTaskTO > 0) nTaskTO--;
            else if(nTaskTO == 0)
            {
//                printf("Tmr ");
                nTaskTO = afHandle[gnState](&stGI, EVT_TIMER, 0);
            }
        }
        
        if(--nDspTO == 0)
        {
            nDspTO = Game_RefreshDsp(&stGI);
        }
    }
}

