
#include "abstract.h"


enum _Color
{
    BLACK,
    RED,
    GREEN,
    BLUE,
    WHITE,
    DUMMY,
    NUM_COLOR
};


void Disp_Init();
void Disp_Refresh(unsigned char* anPixel, int nNumPixel);
void SendWS2811(unsigned char *pColorArray, unsigned short nSize);
