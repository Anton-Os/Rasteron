#include "Toolbox.h"
#include "Font.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
FT_Library freetypeLib;
const char* targetFont = "C:\\AntonDocs\\Codex\\Ao-Project\\Rasteron\\master\\assets\\Tw-Cen-MT.ttf"; // hard path
Rasteron_FormatText textObj;
Rasteron_Image* imageFont;

void init(){
	initFreeType(&freetypeLib);
	// Set font properties and perform font/baking operations
	textObj.bkColor = 0xFF00DDFF;
	textObj.fgColor = 0xFFAA00AA;
	textObj.fontFileName = targetFont;
	textObj.text = "A";
	imageFont = bakeImgText(&textObj, &freetypeLib, 200, 1100);
}

void cleanup() {
	deleteImg(imageFont);
	cleanupFreeType(&freetypeLib);
}

#ifdef _WIN32

BITMAP winBmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {		
		init();

		winBmap = createWinBmapRaw(imageFont->height, imageFont->width, imageFont->data);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &winBmap);
	}
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam); 
	}
	return 0;
}

#elif defined __linux__

void unixProc(){
	UnixContext context;
	createWindow(&context, "Typo");
	init();
}

#endif

int main(int argc, char** argv) {

	srand(time(NULL));
#ifdef _WIN32
	createWindow(wndProc, "Typo");
#elif defined __linux__
	unixProc();
#endif
	eventLoop();

	cleanup(); // cleanup step
	return 0;
}