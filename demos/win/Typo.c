#define RASTERON_ENABLE_PLUGIN
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
FT_Library freetypeLib;
const char* fontPath = FONTS_DIR;
const char* fontName = "Tw-Cen-MT.ttf";
// const char* fontName = "New-Tegomin.ttf";
// const char* fontName = "MajorMonoDisplay.ttf";
char targetFontPath[1024];
Rasteron_FormatText textObj;

Rasteron_Image* fontImage;
Rasteron_Image* flipImage;

void genFontFilePath() {
	strcpy(targetFontPath, fontPath);
	strcat(targetFontPath, "\\");
	strcat(targetFontPath, fontName);
	fixPathDashes(&targetFontPath);
}

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		// bmap = createWinBmap(fontImage);
		bmap = createWinBmap(flipImage);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap);
	}
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam); 
	}
	return 0;
}

int main(int argc, char** argv) {
	genFontFilePath();

	// Generation Step

	initFreeType(&freetypeLib);
	textObj.bkColor = genRandColorVal();
	textObj.fgColor = genRandColorVal();
	textObj.fileName = &targetFontPath;
	textObj.text = "i";

	fontImage = bakeTextReg(&freetypeLib, &textObj);
	flipImage = createImgFlip(fontImage, FLIP_Clock);

	// Event Loop

	createWindow(wndProc, "Typo");
	eventLoop();

	// Cleanup Step

	deleteImg(fontImage);
	deleteImg(flipImage);
	cleanupFreeType(&freetypeLib);

	return 0;
}