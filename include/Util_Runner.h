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

Rasteron_Image* _savedImg = NULL;
Rasteron_Image* _outputImg;

#ifdef RASTERON_ENABLE_ANIM
Rasteron_Queue* _mainQueue;
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

unsigned elapseSecs = 0;

unsigned _dimens[2] = { 1, 1 };

// --------------------------------   Unimplemented Functions for Demo    -------------------------------- //

void _onKeyEvent(char key);
void _onPressEvent(double x, double y);
void _onTickEvent(unsigned secs);
typedef void (*argCallback)(int argc, char** argv);

void parseInput(char lastInput){
    if(isdigit(lastInput)){
        printf("Parsing numeric input for %d", lastInput);
        // TODO: Change Properties
    }
    else if(lastInput == '-'){ if(_dimens[0] > 0) _dimens[0]--; if(_dimens[1] > 0) _dimens[1]--; }
    else if(lastInput == '='){ if(_dimens[0] < 20) _dimens[0]++; if(_dimens[1] < 20) _dimens[1]++; }
}

// --------------------------------   Porting layer for Demo    -------------------------------- //

#ifdef _WIN32

BITMAP bmap;

void CALLBACK wndTimerCallback(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime){ 
    elapseSecs++;
    if(_onTickEvent != NULL) _onTickEvent(elapseSecs);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		if(_outputImg == NULL) _outputImg = solidImgOp((ImageSize){ 1024, 1024}, 0xFFFFFF00);
        if(_onTickEvent != NULL) SetTimer(hwnd, 0, 1000, &wndTimerCallback);
		bmap = createWinBmap(_outputImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, FALSE);

        // TODO: Save image on special sequence
        if(_onKeyEvent != NULL){
            parseInput((char)wParam);
            _onKeyEvent(wParam);
            if(_outputImg != NULL){
                if(_savedImg != NULL) RASTERON_DEALLOC(_savedImg);
                _savedImg = copyImgOp(_outputImg);
            }
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
        if(_outputImg != NULL && _onTickEvent != NULL) bmap = createWinBmap(_outputImg);
    }
    case (WM_LBUTTONDOWN): { if(message == WM_LBUTTONDOWN && _onPressEvent != NULL){
        _onPressEvent((double)GET_X_LPARAM(lParam) / RASTERON_WIN_WIDTH, (double)GET_Y_LPARAM(lParam) / RASTERON_WIN_HEIGHT);
    }}
    case (WM_RBUTTONDOWN): { if(message == WM_RBUTTONDOWN && _onPressEvent != NULL){
        _onPressEvent((double)GET_X_LPARAM(lParam) / RASTERON_WIN_WIDTH, (double)GET_Y_LPARAM(lParam) / RASTERON_WIN_HEIGHT);
    }}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
    }
}

#elif defined __linux__

Platform_Context unixContext;

void unixProc(char lastKey, double cursorPos[2]){
	if(_onKeyEvent != NULL){
        parseInput(lastKey);
        _onKeyEvent(lastKey);
        if(_outputImg != NULL){
            if(_savedImg != NULL) RASTERON_DEALLOC(_savedImg);
            _savedImg = copyImgOp(_outputImg);
        }
    }
	if(_onPressEvent != NULL) _onPressEvent(cursorPos[0] / RASTERON_WIN_WIDTH, cursorPos[1] / RASTERON_WIN_HEIGHT);
	// if(_onTickEvent != NULL) // TODO: Track timer and perform updates

	if(_outputImg != NULL){
		XImage* bmap = createUnixBmap(&unixContext, _outputImg);
		drawUnixBmap(&unixContext, bmap);
		// XDestroyImage(bmap); // destroy after creation
	}
}

#endif

// --------------------------------   Callable Methods for Demo    -------------------------------- //

static void saveToFile(const Rasteron_Image* image){ // TODO: Support more image types
    assert(image != NULL);

    char fileName[1024];

    strcpy(fileName, image->name);
    strcat(fileName, ".png");

    unsigned short iters = 0;
    while(access(fileName, F_OK) == 0){
        iters++;

        char* tempFileName = "";
        strncpy(tempFileName, fileName, strlen(fileName) - ((iters > 1)? 6 : 4));

        char newFileName[1024];
        sprintf(newFileName, "%s-%d", tempFileName, iters);
        strcat(newFileName, ".png");

        strcpy(fileName, newFileName);
    }
    writeFileImageRaw(fileName, IMG_Png, image->height, image->width, image->data);
}

void _run(int argc, char** argv, argCallback callback){
    if(callback != NULL) callback(argc, argv);
#ifdef _WIN32
    createWindow(wndProc, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);
    eventLoop(NULL);
#elif defined __linux__
    puts("Window Creation");
    createWindow(&unixContext, RASTERON_WIN_NAME, RASTERON_WIN_WIDTH, RASTERON_WIN_HEIGHT);
    puts("Event Loop");
    eventLoop(unixContext.display, unixContext.window, unixProc);
#endif
}
