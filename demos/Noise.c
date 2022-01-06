#include "Noise.h"
#include "Pattern.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions

Rasteron_SeedTable seedTable;
Rasteron_ColorPointTable colorPtTable;
Rasteron_NoiseGradientTable noiseGradientTable;

Rasteron_Image* blankImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* randNoiseImg2;
Rasteron_Image* latticeNoiseImg;
Rasteron_Image* scatterImg;
Rasteron_Image* splashImg;
Rasteron_Image* proxCellImg;

void genImages(){
	addWeightedSeed(&seedTable, 0xFFFFEECC, 0.1f);
	addWeightedSeed(&seedTable, 0xFFAADDEE, 0.2f);
	addWeightedSeed(&seedTable, 0xFFAADDEE, 0.3f);
	addWeightedSeed(&seedTable, 0xFFAAAAFF, 0.4f);

	addColorPoint(&colorPtTable, 0xFFFFEECC, 0.1f, 0.1f);
	addColorPoint(&colorPtTable, 0xFFAADDEE, 0.2f, 0.5f);
	addColorPoint(&colorPtTable, 0xFFDDEECC, 0.5f, 0.2f); 
	addColorPoint(&colorPtTable, 0xFFAAAAFF, 0.4f, 0.4f);

	noiseGradientTable.xCellDivs = 5;
	noiseGradientTable.yCellDivs = 5;

	blankImg = createImgBlank(1200, 1100, 0xFF73e5ff);
	randNoiseImg = createWhiteNoiseImg(0xFFFFFFFF, 0xFF000000, blankImg);
	randNoiseImg2 = createGradientNoiseImg(0xFFFFFFFF, 0xFF00FFFF, blankImg);
	scatterImg = createImgScatter(blankImg, 0xFFFF00FF, 0.1);
	splashImg = createImgSplash(blankImg, &seedTable);
	proxCellImg = createImgProxCell(blankImg, &colorPtTable);
}

void cleanup() {
	deleteImg(blankImg);
	deleteImg(randNoiseImg);
	deleteImg(randNoiseImg2);
	deleteImg(scatterImg);
	deleteImg(splashImg);
	deleteImg(proxCellImg);
}

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		seedRandGen();
		genImages();

		bmap = createWinBmapRaw(randNoiseImg->height, randNoiseImg->width, randNoiseImg->data);
		// bmap = createWinBmapRaw(scatterImg->height, scatterImg->width, scatterImg->data);
		// bmap = createWinBmapRaw(splashImg->height, splashImg->width, splashImg->data);
		// bmap = createWinBmapRaw(proxCellImg->height, proxCellImg->width, proxCellImg->data);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap);
	}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#elif defined __linux__

void unixProc(){
	UnixContext context;
	createWindow(&context, "Noise");
	seedRandGen();
	genImages();

	XImage* unixBmap = createUnixBmapRaw(context, scatterImg->width, scatterImg->height, scatterImg->data);
	drawUnixBmap(context, unixBmap);
}

#endif

int main(int argc, char** argv) {

#ifdef _WIN32
	createWindow(wndProc, "Noise");
#elif defined __linux__
	unixProc();
#endif
	eventLoop();

	cleanup(); // cleanup step
	return 0;
}