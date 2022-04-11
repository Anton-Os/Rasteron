#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

Rasteron_SeedTable seedTable;
Rasteron_ColorPointTable colorPtTable;
Rasteron_GradientNoise noiseGradientTable = { 3, 3, 0xFF0000FF, 0xFF00FF00 };
// Rasteron_GradientNoise noiseGradientTable = { 12, 12, 0xFFFF00FF, 0xFF00FFFF };
// Rasteron_GradientNoise noiseGradientTable = { 64, 64, 0xFF000000, 0xFFFFFFFF };

Rasteron_Image* blankImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* randNoiseImg2;
Rasteron_Image* seededImg;
Rasteron_Image* paletteImg;
Rasteron_Image* proxCellImg;

BITMAP bmap1, bmap2, bmap3;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		// bmap = createWinBmap(randNoiseImg);
		bmap1 = createWinBmap(randNoiseImg2);
		// bmap = createWinBmap(seededImg);
		// bmap = createWinBmap(paletteImg);
		// bmap = createWinBmap(proxCellImg);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap1);
	}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {
	srand(time(NULL));
	
	// Genertation Step

	addWeightedSeed(&seedTable, 0xFFFFEECC, 0.1f);
	addWeightedSeed(&seedTable, 0xFFAADDEE, 0.2f);
	addWeightedSeed(&seedTable, 0xFFAADDEE, 0.3f);
	addWeightedSeed(&seedTable, 0xFFAAAAFF, 0.4f);

	addColorPoint(&colorPtTable, 0xFFFFEECC, 0.1f, 0.1f);
	addColorPoint(&colorPtTable, 0xFFAADDEE, 0.2f, 0.5f);
	addColorPoint(&colorPtTable, 0xFFDDEECC, 0.5f, 0.2f); 
	addColorPoint(&colorPtTable, 0xFFAAAAFF, 0.4f, 0.4f);

	blankImg = createImgBlank(1100, 1200, 0xFF73e5ff);
	randNoiseImg = createWhiteNoiseImg(blankImg, 0xFFFF0000, 0xFF0000FF);
	randNoiseImg2 = createGradientNoiseImg(blankImg, &noiseGradientTable);
	seededImg = createImgSeedRaw(blankImg, 0xFFFF00FF, 0.1);
	paletteImg = createImgSeedWeighted(blankImg, &seedTable);
	proxCellImg = createImgProxim(blankImg, &colorPtTable);

	// Event Loop

	createWindow(wndProc, "Noise");
	eventLoop();

	// Cleanup Step

	deleteImg(blankImg);
	deleteImg(randNoiseImg);
	deleteImg(randNoiseImg2);
	deleteImg(seededImg);
	deleteImg(paletteImg);
	deleteImg(proxCellImg);

	return 0;
}