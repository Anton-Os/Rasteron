#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
Rasteron_SeedTable seedTable;

Rasteron_Image* blankImg;
Rasteron_Image* seededImg;
Rasteron_Image* seededImg2;
Rasteron_Image* patternImg1;
Rasteron_Image* patternImg2;
Rasteron_Image* patternImg3;

// Start with simple black and white patterns
unsigned callback2(unsigned right, unsigned left) {
	return ZERO_COLOR;
}

unsigned callback4(unsigned bottom, unsigned right, unsigned left, unsigned top) {
	return ZERO_COLOR; // for testing
}

unsigned callback8(unsigned br, unsigned b, unsigned bl, unsigned r, unsigned l, unsigned tr, unsigned t, unsigned tl) {
	return ZERO_COLOR; // for testing
}

void genImages() {
	addSeed(&seedTable, 0xFFFF0000); // red
	addSeed(&seedTable, 0xFF00FF00); // green
	addSeed(&seedTable, 0xFF0000FF); // blue

	blankImg = createImgBlank(1100, 1200, BLACK_COLOR);
	seededImg = createImgSeedRaw(blankImg, WHITE_COLOR, 0.05);
	seededImg2 = createImgSeedWeighted(blankImg, &seedTable);

	patternImg1 = createCellPatImg2(seededImg, callback2);
	patternImg2 = createCellPatImg4(seededImg, callback4);
	patternImg3 = createCellPatImg8(seededImg2, callback8);
}

void cleanup() {
	deleteImg(blankImg);
	deleteImg(seededImg);
	deleteImg(seededImg2);

	deleteImg(patternImg1);
	deleteImg(patternImg2);
	deleteImg(patternImg3);
}

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		genImages();
		bmap = createWinBmap(patternImg1);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap);
	}
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#elif defined __linux__

void unixProc(){
	UnixContext context;
	createWindow(&context, "Cellwise");
	genImages();
}

#endif

int main(int argc, char** argv) {

#ifdef _WIN32
	createWindow(wndProc, "Cellwise");
#elif defined __linux__
	unixProc();
#endif
	eventLoop();

	cleanup(); // cleanup step

	return 0;
}