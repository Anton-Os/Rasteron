#include "Util_OS.h"

#ifndef RASTERON_WIN_NAME
    #define RASTERON_WIN_NAME "Demo"
#endif

#ifndef RASTERON_WIN_WIDTH
    #define RASTERON_WIN_WIDTH 1024
#endif

#ifndef RASTERON_WIN_HEIGHT
    #define RASTERON_WIN_HEIGHT 1024
#endif

/* extern Rasteron_Image* canvasImg;
extern void (*keyEvent)(char key);
extern void (*mouseEvent)(double x, double y);
extern void (*timerEvent)(unsigned secs); */

static unsigned elapseSecs = 0;

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
		if(canvasImg == NULL) canvasImg = solidImgOp((ImageSize){ 1024, 1024}, 0xFFFFFF00);
        if(timerEvent != NULL) SetTimer(hwnd, 0, 1000, &wndTimerCallback);
		bmap = createWinBmap(canvasImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, FALSE);

        if(keyEvent != NULL){
            keyEvent(wParam);
            bmap = createWinBmap(canvasImg);
	    }
    }
	case (WM_PAINT): { 
		InvalidateRect(hwnd, &rect, TRUE);
		drawWinBmap(hwnd, &bmap); 
	}
    case (WM_TIMER): {
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, FALSE);
        if(canvasImg != NULL && timerEvent != NULL) bmap = createWinBmap(canvasImg);
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

// Passes into main function

void inputLoop(eventLoopCallback callback){
#ifdef _WIN32
    createWindow(wndProc, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);
	eventLoop(callback);
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);

    XImage* bmap = createUnixBmap(&platformContext, canvasImg);
    eventLoop(platformContext.display, callback);
#endif
}