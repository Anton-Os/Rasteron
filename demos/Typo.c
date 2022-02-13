#include "Toolbox.h"
#include "Font.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
FT_Library freetypeLib;
const char* fontPath = FONTS_DIR;
const char* fontName = "Tw-Cen-MT.ttf";
// const char* fontName = "New-Tegomin.ttf";
// const char* fontName = "MajorMonoDisplay.ttf";
char targetFontPath[1024];
Rasteron_FormatText textObj;
Rasteron_Image* imageFont;

void genFontFilePath() {
	strcpy(targetFontPath, fontPath);
	strcat(targetFontPath, "\\");
	strcat(targetFontPath, fontName);
	fixPathDashes(&targetFontPath);
}

void init(){
	genFontFilePath();

	initFreeType(&freetypeLib);
	textObj.bkColor = genRandColorVal();
	textObj.fgColor = genRandColorVal();
	textObj.fileName = &targetFontPath;
	textObj.text = "Hello World";
	imageFont = bakeImgText(&textObj, &freetypeLib, 200);
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

		winBmap = createWinBmap(imageFont);
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