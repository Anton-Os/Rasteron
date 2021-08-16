#include "Noise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions

Rasteron_SeedTable seedTable;
Rasteron_Image* blankImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* randNoiseImg2;
Rasteron_Image* latticeNoiseImg;
Rasteron_Image* scatterImg;
Rasteron_Image* perlinNoiseImg;
Rasteron_Image* splatterImg;
Rasteron_Image* proxCellImg;

Rasteron_NoiseGradientLattice noiseGradientLattice;
Rasteron_ColorPointTable colorPointTable;

BITMAP bmap;

void cleanup() {
	deleteImg(blankImg);
	deleteImg(randNoiseImg);
	deleteImg(randNoiseImg2);
	deleteImg(perlinNoiseImg);
	deleteImg(scatterImg);
	deleteImg(proxCellImg);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		blankImg = createImgBlank(1200, 1000, 0xFF73e5ff);
		seedRandGen();
		// randNoiseImg = createRandNoiseImg(0xFFFFFF00, 0xFF73e5ff, blankImg);
		randNoiseImg = createRandNoiseImg(0xFFFFFFFF, 0xFF000000, blankImg);
		randNoiseImg2 = createRandNoiseImg(0xFFFFFFFF, 0xFF00FFFF, blankImg);
		scatterImg = createImgScatter(randNoiseImg2, 0xFFFF00FF, 0.1);

		noiseGradientLattice.xCellDivs = 4;
		noiseGradientLattice.yCellDivs = 4;
		perlinNoiseImg = createPerlinNoiseImg(&noiseGradientLattice, blankImg);

		addColorPoint(&colorPointTable, 0xFFFFFF00, 0.4, 0.1);
		addColorPoint(&colorPointTable, 0xFF00FF00, 0.3, 0.6);
		addColorPoint(&colorPointTable, 0xFF00FFFF, 0.8, 0.4);
		proxCellImg = createImgProxCell(blankImg, &colorPointTable);

		// bmap = createWinBmapRaw(blankImg->width, blankImg->height, blankImg->data);
		// bmap = createWinBmapRaw(scatterImg->width, scatterImg->height, scatterImg->data);
		// bmap = createWinBmapRaw(proxCellImg->width, proxCellImg->height, proxCellImg->data);
		bmap = createWinBmapRaw(perlinNoiseImg->width, perlinNoiseImg->height, perlinNoiseImg->data);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap);
	}
	case (WM_CLOSE): {
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {

	createWindow(wndProc, "Noise");
	eventLoop();

	cleanup(); // cleanup step
	
	return 0;
}