#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

GradientLattice gradientTable = { 3, 3, 0xFF0000FF, 0xFF00FF00 };
// Rasteron_GradientNoise gradientTable = { 12, 12, 0xFFFF00FF, 0xFF00FFFF };
// Rasteron_GradientNoise gradientTable = { 64, 64, 0xFF000000, 0xFFFFFFFF };

Rasteron_Image* solidImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* randNoiseImg2;

void cleanup(){
	deleteImg(solidImg);
	deleteImg(randNoiseImg);
	deleteImg(randNoiseImg2);
}

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

	solidImg = createSolidImg((ImageSize){ 1100, 1200 }, 0xFF73e5ff);
	randNoiseImg = createNoiseImg_white((ImageSize) { 1100, 1200 }, 0xFFFF0000, 0xFF0000FF);
	randNoiseImg2 = createNoiseImg_gradient((ImageSize) { 1100, 1200 }, gradientTable);

	// Event Loop

	createWindow(wndProc, "Noise");
	eventLoop();

	// Cleanup Step

	cleanup();

	return 0;
}