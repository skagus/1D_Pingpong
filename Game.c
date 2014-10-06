#include <stdio.h>
#include <string.h>
#include "Game.h"
#include "Disp.h"

State gnState;

void Game_Init(GameInfo* pstGI)
{
    memset(pstGI, 0x00, sizeof(*pstGI));
    gnState = ST_INC_SCORE;
}


void Inc_LED(GameInfo* pstGI)
{
    /** Make LED array for INC_SCORE state */
    int i;
    memset(pstGI->anLED, 0x00, sizeof(pstGI->anLED));

    for(i = 0; i< pstGI->nPos; i++)
    {
        if(pstGI->anScore[PLAYER_L] > i)
        {
            pstGI->anLED[i] |= KEY_L;
        }
        if(pstGI->anScore[PLAYER_R] > i)
        {
            pstGI->anLED[NUM_LED - 1 - i] |= KEY_R;
        }
    }
    pstGI->bRefresh = !FALSE;
}
/**
Show scores.
*/
int IncScore_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag)
{
    int bDone = !FALSE; // TRUE.
    int nTO = 0;
    if(nEvt == EVT_TIMER)
    {
        if(pstGI->nPos <= pstGI->anScore[PLAYER_L])
        {
            bDone = FALSE;
        }
        else if(pstGI->nPos <= pstGI->anScore[PLAYER_R])
        {
            bDone = FALSE;
        }

        if(bDone == FALSE)
        {
            Inc_LED(pstGI);
            pstGI->nPos++;
        }
        else
        {
            gnState = ST_BLINK_SCORE;
            pstGI->nPos = 0;
        }
        nTO = INC_PERIOD;
    }
    else if(nEvt == EVT_KEY)  // Don't response key event.
    {
        // Nothing to do..
    }
    return nTO;
}


void Blink_LED(GameInfo* pstGI)
{
    /** Make LED array for INC_SCORE state */

    if((pstGI->nPos & 0x01) == 0)  // even number.
    {
        memset(pstGI->anLED, BLACK, sizeof(pstGI->anLED));
    }
    else
    {
        int i;
        int max;
        memset(pstGI->anLED, BLACK, sizeof(pstGI->anLED));
        max = pstGI->anScore[0] > pstGI->anScore[1] ? pstGI->anScore[0] : pstGI->anScore[1];
        for(i = 0; i< max; i++)
        {
            if(pstGI->anScore[PLAYER_L] > i)
            {
                pstGI->anLED[i] |= KEY_L;
            }
            if(pstGI->anScore[PLAYER_R] > i)
            {
                pstGI->anLED[NUM_LED - i - 1] |= KEY_R;            }
        }
    }
    pstGI->bRefresh = !FALSE;
}

int BlinkScore_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag)
{
    int nTO = 0;
    if(nEvt == EVT_TIMER)
    {
        if(pstGI->nPos < MIN_BLINK)
        {
            Blink_LED(pstGI);
            pstGI->nPos++;
            nTO = BLINK_PERIOD;
        }
        else if(pstGI->nPos == MIN_BLINK)
        {
            pstGI->nPos = (pstGI->nPos + 1) | 0x01;
            Blink_LED(pstGI);
        }
        else    
        {
            gnState = ST_WAIT_GAME;
            nTO = RUN_PERIOD;
            pstGI->nPos = ZONE_CENTER;
            pstGI->nSpeed = WAIT_SPEED;
        }
    }
    else if(nEvt == EVT_KEY && pstGI->nPos > MIN_BLINK)
    {
        gnState = ST_WAIT_GAME;
        nTO = RUN_PERIOD;
        pstGI->nPos = ZONE_CENTER;
        pstGI->nSpeed = WAIT_SPEED;
    }
    return nTO;
}

int GetReactSpeed(int nPos, int nOldSpeed)
{
    int nSpeed;
    if( nPos > ZONE_CENTER)
    {
        nSpeed = -((nPos - ZONE_CENTER) >> 4) - 10 - (nOldSpeed/2);
    }
    else
    {
        nSpeed = ((ZONE_CENTER - nPos) >> 4) + 10 - (nOldSpeed/2);
    }

    return nSpeed;
}

void Game_LED(GameInfo* pstGI)
{
    /** Make LED array for INC_SCORE state */
    int nLEDPos;
    memset(pstGI->anLED, 0x00, sizeof(pstGI->anLED));

    nLEDPos = pstGI->nPos / NUM_PER_LED;
//    nLEDPos = pstGI->nPos >> SHIFT_NUM_LED;
    pstGI->anLED[nLEDPos] = KEY_L | KEY_R;

    pstGI->bRefresh = !FALSE;
}

int WaitGame_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag)
{
    if(nEvt == EVT_TIMER)
    {
        int nMin;
        int nMax;

        if(pstGI->nPrevWinner == PLAYER_R)
        {
            nMin = ZONE_CENTER - SIZE_RDY_ZONE;
            nMax = ZONE_CENTER - 1;
        }
        else
        {
            nMin = ZONE_CENTER + 1;
            nMax = ZONE_CENTER + SIZE_RDY_ZONE;
        }

        pstGI->nPos += pstGI->nSpeed;

        if(pstGI->nPos >= nMax)
        {
            pstGI->nPos = nMax;
            pstGI->nSpeed = -(pstGI->nSpeed);
        }
        else if(pstGI->nPos < nMin)
        {
            pstGI->nPos = nMin;
            pstGI->nSpeed = -(pstGI->nSpeed);
        }
        Game_LED(pstGI);
    }
    else if(nEvt == EVT_KEY)
    {
        if((nFlag & KEY_R) && (pstGI->nPrevWinner == PLAYER_L))
        {
            pstGI->nSpeed = GetReactSpeed(pstGI->nPos, pstGI->nSpeed);
            gnState = ST_RUN_GAME;
        }
        else if ((nFlag & KEY_L) && (pstGI->nPrevWinner == PLAYER_R))
        {
            pstGI->nSpeed = GetReactSpeed(pstGI->nPos, pstGI->nSpeed);
            gnState = ST_RUN_GAME;
        }
    }
    return RUN_PERIOD;
}

/** ?????? ¹ö?°?? ´­·??ö¸é, game ½?????. */
int RunGame_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag)
{
    int nTO = 0;
    if(nEvt == EVT_TIMER)
    {
        int nMin = 0;
        int nMax = SIZE_GAME_ZONE;
        pstGI->nPos += pstGI->nSpeed;

        nTO = RUN_PERIOD;
        if(pstGI->nPos >= nMax)
        {
            pstGI->nPrevWinner = PLAYER_L;
            gnState = ST_INC_SCORE;
            pstGI->nPos = 0;
            pstGI->anScore[PLAYER_L]++;
        }
        else if(pstGI->nPos < nMin)
        {
            pstGI->nPrevWinner = PLAYER_R;
            pstGI->anScore[PLAYER_R]++;
            gnState = ST_INC_SCORE;
            pstGI->nPos = 0;
        }
        else
        {
            Game_LED(pstGI);
        }
    }
    else if(nEvt == EVT_KEY)
    {
        if(    (pstGI->nSpeed < 0)
            && (pstGI->nPos < ZONE_CENTER)
            && (nFlag & KEY_L) )
        {   // In Left zone.
            pstGI->nSpeed = GetReactSpeed(pstGI->nPos, pstGI->nSpeed);
        }
        else if((pstGI->nSpeed > 0)
             && (pstGI->nPos > ZONE_CENTER)
             && (nFlag & KEY_R) )
        {   // In Right zone.
            pstGI->nSpeed = GetReactSpeed(pstGI->nPos, pstGI->nSpeed);
        }
    }
    return nTO;
}




int Game_RefreshDsp(GameInfo* pstGI)
{
//    printf("%d %d %d %d\n", gnState, pstGI->nPos, pstGI->nStep, pstGI->nPrevWinner);

    if(pstGI->bRefresh)
    {
        Disp_Refresh(pstGI->anLED, NUM_LED);
        pstGI->bRefresh = 0;
    }
    return DSP_PERIOD;
}

