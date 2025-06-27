#define RASTERON_ENABLE_ANIM

#include "Util_OS.h"

// --------------------------------   Directives for Demo    -------------------------------- //

#ifndef RASTERON_WIN_NAME
#define RASTERON_WIN_NAME "Rasteron"
#endif

#ifndef RASTERON_WIN_WIDTH
#define RASTERON_WIN_WIDTH 1024
#endif

#ifndef RASTERON_WIN_HEIGHT
#define RASTERON_WIN_HEIGHT 1024
#endif

#define PRIMITIVES_MAX 1024
#define TABLES_MAX 512
#define IMAGES_MAX 4096

// --------------------------------   Objects for Demo    -------------------------------- //

extern ColorSwatch _swatch;

extern Rasteron_Image* _savedImg;
extern Rasteron_Image* _outputImg;

#ifdef RASTERON_ENABLE_ANIM
extern Rasteron_Queue* _mainQueue;
#endif

/*
typedef struct {
    Rasteron_Image* images[IMAGES_MAX];

    ColorSwatch swatches[PRIMITIVES_MAX];
    PixelPoint pixelPoints[PRIMITIVES_MAX];
    PixelPointTable pixelPointTables[TABLES_MAX];
    ColorPoint colorPoints[PRIMITIVES_MAX];
    ColorPointTable colorPointTables[TABLES_MAX];
    ColorGrid grids[PRIMITIVES_MAX];
    ColorGridTable gridTables[TABLES_MAX];
    
    Rasteron_Sprite* sprites[512]; // unsigned short spriteIndex; // for tracking active sprites
    Rasteron_Heightmap* heightmaps[512]; // unsigned short heightmapIndex; // for tracking active heightmaps

#ifdef RASTERON_ENABLE_ANIM
    Rasteron_Queue* queues[256]; // unsigned short queueIndex; // for tracking active queues
#endif
#ifdef RASTERON_ENABLE_FONT
    Rasteron_Text textObjs[1024];
    Rasteron_Message messageObjs[256];
#endif
} _catalouge;  
*/

extern unsigned elapseSecs;

extern unsigned _dimens[2];

// --------------------------------  Functions for Demo    -------------------------------- //

void _onKeyEvent(char key);
void _onPressEvent(double x, double y);
void _onTickEvent(unsigned secs);

typedef Rasteron_Image* (*imageArgCallback)(char*);

imageArgCallback _outputCallback;

void parseInput(char lastInput);

// --------------------------------   Porting layer for Demo    -------------------------------- //

#ifdef _WIN32

void CALLBACK wndTimerCallback(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

#elif defined __linux__

void unixProc(char lastKey, double cursorPos[2]);

#endif

// --------------------------------   Callable Methods for Demo    -------------------------------- //

void saveToFile(const Rasteron_Image* image, enum IMG_FileFormat format);

char* parseArgs(int argc, char** argv);

void _run(int argc, char** argv, imageArgCallback callback);
