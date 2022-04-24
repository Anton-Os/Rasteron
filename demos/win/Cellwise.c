#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
Rasteron_Swatch darkSwatch, lightSwatch;
Rasteron_SeedTable seedTable1, seedTable2;

Rasteron_Image* solidImg;

Rasteron_Image* seededImg;
Rasteron_Image* seededImg2;

Rasteron_Image* patternImg;
Rasteron_Image* patternImg2;
Rasteron_Image* patternImgVert;
Rasteron_Image* patternImgHorz;

#define SEED_COLOR 0xFFFFFFFF
#define RESULT_COLOR 0xFF00FF00

// Start with simple black and white patterns
unsigned callback2(unsigned target, unsigned right, unsigned left) {
	// return blend(left, right, 0.5);
	// return fuse(left, right, 0.5f);

	if (left == SEED_COLOR) return SEED_COLOR;
	else if (right == SEED_COLOR) return BLACK_COLOR;
	else return ZERO_COLOR;
}

unsigned callback8(unsigned target, unsigned nebrs[NEBR_COUNT]) {
	/* if(t == SEED_COLOR) return 0xFFFF0000; else if (b == SEED_COLOR) return 0xFFFFFF00;
	else if(tl == SEED_COLOR) return 0xFF00FF00; else if (bl == SEED_COLOR) return 0xFFFF00FF;
	else if(tr == SEED_COLOR) return 0xFF0000FF; else if (br == SEED_COLOR) return 0xFF00FFFF;
	else return ZERO_COLOR; */

	return ZERO_COLOR;
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

	darkSwatch = createSwatch(BLACK_COLOR, 0x25);
	lightSwatch = createSwatch(WHITE_COLOR, 0x25);
	seedTable1 = createSeedTable(&lightSwatch);
	seedTable2 = createSeedTable(&darkSwatch);

	solidImg = createImgSolid((ImageSize){ 1100, 1200 }, BLACK_COLOR);

	seededImg = createImgSeedRaw(solidImg, SEED_COLOR, 0.01);
	seededImg2 = createImgSeedWeighted(solidImg, &seedTable2);

	patternImg = createPatternImg_nebr(seededImg, callback8);
	patternImg2 = createPatternImg_iter(seededImg, callback8, 4);
	patternImgHorz = createPatternImg_horz(seededImg, callback2);
	patternImgVert = createPatternImg_vert(seededImg, callback2);

	// Event Loop

	createWindow(wndProc, "Cellwise");
	eventLoop();

	// Cleanup Step

	deleteImg(solidImg);

	deleteImg(seededImg);
	deleteImg(seededImg2);

	deleteImg(patternImg);
	deleteImg(patternImg2);
	deleteImg(patternImgHorz);
	deleteImg(patternImgVert);

	return 0;
}