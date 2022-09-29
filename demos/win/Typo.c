#define RASTERON_ENABLE_PLUGIN
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

FT_Library freetypeLib;
const char* fontPath = ASSETS_DIR;
const char* fontName = "Tw-Cen-MT.ttf";
// const char* fontName = "New-Tegomin.ttf";
// const char* fontName = "MajorMonoDisplay.ttf";
char fullFontPath[1024];
Rasteron_FormatText textObj;

Rasteron_Image* fontImage;
Rasteron_Image* flipImage;

void genImages(){
	textObj.bkColor = genRandColorVal();
	textObj.fgColor = genRandColorVal();
	textObj.fileName = &fullFontPath;
	textObj.text = "Rasteron is Dope!";

	fontImage = bakeTextReg(&freetypeLib, &textObj);
	flipImage = createFlipImg(fontImage, FLIP_Clock);
}

void cleanup(){
	deleteImg(fontImage);
	deleteImg(flipImage);
}

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): { bmap = createWinBmap(fontImage); }
	case (WM_PAINT): { drawWinBmap(hwnd, &bmap); }
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam); 
	}
	return 0;
}

int main(int argc, char** argv) {
	// Generation Step

	initFreeType(&freetypeLib);
	genFullFilePath(fontName, &fullFontPath);
	genImages();

	// Event Loop

	createWindow(wndProc, "Typo");
	eventLoop();

	// Cleanup Step

	cleanup();
	cleanupFreeType(&freetypeLib);

	return 0;
}