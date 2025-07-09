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
