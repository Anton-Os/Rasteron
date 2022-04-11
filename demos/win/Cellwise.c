#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
Rasteron_SeedTable seedTable;
Rasteron_Swatch darkSwatch, lightSwatch;

Rasteron_Image* blankImg;

Rasteron_Image* seededImg;
Rasteron_Image* seededImg2;

Rasteron_Image* patternImg1;
Rasteron_Image* patternImg2;
Rasteron_Image* patternImgVert;
Rasteron_Image* patternImgHorz;

#define SEED_COLOR 0xFFFFFFFF

// Start with simple black and white patterns
unsigned callback2(unsigned right, unsigned left) {
	// return blend(left, right, 0.5);
	return fuse(left, right, 0.9f);
}

unsigned callback8(unsigned br, unsigned b, unsigned bl, unsigned r, unsigned l, unsigned tr, unsigned t, unsigned tl) {
	if(t == SEED_COLOR) return 0xFFFF0000; else if (b == SEED_COLOR) return 0xFFFFFF00;
	else if(tl == SEED_COLOR) return 0xFF00FF00; else if (bl == SEED_COLOR) return 0xFFFF00FF;
	else if(tr == SEED_COLOR) return 0xFF0000FF; else if (br == SEED_COLOR) return 0xFF00FFFF;
	else return ZERO_COLOR;
}

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap = createWinBmap(patternImgHorz);
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
	// Genertation Step

	darkSwatch = createSwatch(BLACK_COLOR, 0x33);
	lightSwatch = createSwatch(WHITE_COLOR, 0x33);
	// seedTable = createSeedTable(&lightSwatch);

	blankImg = createImgBlank(1100, 1200, BLACK_COLOR);

	seededImg = createImgSeedRaw(blankImg, SEED_COLOR, 0.01);
	seededImg2 = createImgSeedWeighted(blankImg, &seedTable);

	patternImg1 = createCellPatImg2(seededImg, callback2);
	patternImg2 = createCellPatImg8(seededImg, callback8);
	// patternImgVert = createVertPatImg(seededImg, callback2);
	patternImgHorz = createHorzPatImg(seededImg, callback2);

	// Event Loop

	createWindow(wndProc, "Cellwise");
	eventLoop();

	// Cleanup Step

	deleteImg(blankImg);

	deleteImg(seededImg);
	deleteImg(seededImg2);

	deleteImg(patternImg1);
	deleteImg(patternImg2);
	// deleteImg(patternImgVert);
	deleteImg(patternImgHorz);

	return 0;
}