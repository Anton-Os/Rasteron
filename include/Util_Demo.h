#include "Util_OS.h"

// --------------------------------   Preprocessor Directives for Demo    -------------------------------- //

#ifndef RASTERON_WIN_NAME
    #define RASTERON_WIN_NAME "Demo"
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

Rasteron_Image* _outputImg; // final image used for drawing

#ifdef RASTERON_ENABLE_ANIM
Rasteron_Queue* _mainQueue; // main queue used for multiple images
#endif

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

unsigned elapseSecs = 0;

// --------------------------------   Unimplemented Functions for Demo    -------------------------------- //

void keyEvent(char key);
void mouseEvent(double x, double y);
void timerEvent(unsigned secs);

// --------------------------------   Porting layer for Demo    -------------------------------- //

#ifdef _WIN32

BITMAP bmap;

void CALLBACK wndTimerCallback(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime){ 
    elapseSecs++;
    if(timerEvent != NULL) timerEvent(elapseSecs);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		if(_outputImg == NULL) _outputImg = solidImgOp((ImageSize){ 1024, 1024}, 0xFFFFFF00);
        if(timerEvent != NULL) SetTimer(hwnd, 0, 1000, &wndTimerCallback);
		bmap = createWinBmap(_outputImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, FALSE);

        if(keyEvent != NULL){
            keyEvent(wParam);
            bmap = createWinBmap(_outputImg);
	    }
    }
	case (WM_PAINT): { 
		InvalidateRect(hwnd, &rect, TRUE);
		drawWinBmap(hwnd, &bmap); 
	}
    case (WM_TIMER): {
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, FALSE);
        if(_outputImg != NULL && timerEvent != NULL) bmap = createWinBmap(_outputImg);
    }
    case (WM_LBUTTONDOWN): { if(message == WM_LBUTTONDOWN && mouseEvent != NULL){
        // Translate Data from points to coordinates
        mouseEvent(0.0, 0.0);
    }}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
    }
}

#endif

// --------------------------------   Callable Methods for Demo    -------------------------------- //

void inputLoop(eventLoopCallback callback){
#ifdef _WIN32
    createWindow(wndProc, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);
	eventLoop(callback);
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);

    XImage* bmap = createUnixBmap(&platformContext, _outputImg);
    eventLoop(platformContext.display, callback);
#endif
}