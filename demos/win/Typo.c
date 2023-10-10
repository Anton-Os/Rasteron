#define RASTERON_ENABLE_FONT

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

const char* fontName = "Tw-Cen-MT.ttf";
char fullFontPath[1024];
Rasteron_Text textObj;

Rasteron_Image* fontImage;
Rasteron_Image* flipImage;

void genImages(){
	textObj.bkColor = genRandColorVal();
	textObj.fgColor = genRandColorVal();
	textObj.fileName = &fullFontPath;
	textObj.text = "Rasteron is Dope!";

	fontImage = bakeText(&textObj);
	flipImage = createFlipImg(fontImage, FLIP_Clock);
}

void cleanup(){
	free_image(fontImage);
	free_image(flipImage);
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

	srand(time(NULL));
	genFullFilePath(fontName, &fullFontPath);
	genImages();

	// Event Loop

	createWindow(wndProc, "Typo");
	eventLoop();

	// Cleanup Step

	cleanup();

	return 0;
}