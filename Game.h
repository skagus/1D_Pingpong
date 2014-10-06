
#define NUM_LED             (60)
#define NUM_PER_LED			(128)  // Assign 128 in a LED.
#define SIZE_GAME_ZONE      (NUM_LED * NUM_PER_LED)
#define ZONE_CENTER         (SIZE_GAME_ZONE / 2)
#define SIZE_RDY_ZONE		(SIZE_GAME_ZONE / 4)  // LED moving area while ready state.

#define     PLAYER_L    0
#define     PLAYER_R    1
#define     NUM_PLAYER  2

#define KEY_R		(1<< (PLAYER_R))
#define KEY_L		(1<< (PLAYER_L))

#define FALSE		(0)
#if 1
#define DSP_PERIOD      (10)    // Shortest LED refresh period. (ms)
#define RUN_PERIOD      (10)    // Run sequence period.
#define INC_PERIOD      (1000)   // Increase score period.
#define BLINK_PERIOD    (500)   // Blink score period.
#define MIN_BLINK       (4)    // Minimum number of blink count
#else
#define DSP_PERIOD      (1)    // Shortest LED refresh period. (ms)
#define RUN_PERIOD      (1)    // Run sequence period.
#define INC_PERIOD      (2)   // Increase score period.
#define BLINK_PERIOD    (1)   // Blink score period.
#define MIN_BLINK       (4)    // Minimum number of blink count
#endif
#define WAIT_SPEED      (50)
// Never define TRUE.
// Use '== FALSE', '!= FALSE'


//#define DBG

typedef struct _GameInfo
{
// Global variable.
    int anScore[NUM_PLAYER];
    int nPrevWinner;

// Used on ST_INC_SCORE, ST_BLINK_SCORE
    int nStep;

// Used on ST_WAIT_GAME, ST_RUN_GAME
    int nPos;   // LED's position.
    int nSpeed; // Speed of LED.

// Used for LED Task.
    unsigned char anLED[NUM_LED];
    int bRefresh;
} GameInfo;

typedef enum _Event
{
    EVT_TIMER,
    EVT_KEY,
    NUM_EVT,
} Event;

typedef enum _State
{
    ST_INC_SCORE,  // Á¡¼ö¸¦ º¸¿©ÁÜ..
    ST_BLINK_SCORE,
    ST_WAIT_GAME,  // Áø »ç¶÷?Ì ¹öÆ° ´©¸£¸é ½Ã?ÛÇÔ.
    ST_RUN_GAME, // Start
    NUM_STATE
} State;

extern State gnState;

/**
returns timer to call next.
0 means 'do not update timer'
*/
typedef int (*Handler)(GameInfo* pstGI, Event nEvt, unsigned int nFlag);

void Game_Init(GameInfo* pstGI);
int Game_RefreshDsp(GameInfo* pstGI);

int IncScore_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag);
int BlinkScore_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag);
int WaitGame_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag);
int RunGame_Task(GameInfo* pstGI, Event nEvt, unsigned int nFlag);
