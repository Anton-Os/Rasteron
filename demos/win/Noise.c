#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

Lattice lattice1 = { 3, 3, 0xFF0000FF, 0xFF00FF00 };
Lattice lattice2 = { 12, 12, 0xFFFF00FF, 0xFF00FFFF };
Lattice lattice3 = { 64, 64, 0xFF000000, 0xFFFFFFFF };

Rasteron_Image* solidImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* randNoiseImg2;

void genImages() {
	solidImg = createSolidImg((ImageSize){ 1100, 1200 }, 0xFF73e5ff);
	randNoiseImg = createWhiteNoiseImg((ImageSize) { 1100, 1200 }, 0xFFFF0000, 0xFF0000FF);
	randNoiseImg2 = createGradientNoiseImg((ImageSize) { 1100, 1200 }, lattice1);
}

void cleanup(){
	free_image(solidImg);
	free_image(randNoiseImg);
	free_image(randNoiseImg2);
}

BITMAP bmap1, bmap2, bmap3;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap1 = createWinBmap(randNoiseImg);
		bmap2 = createWinBmap(randNoiseImg2);
	}
	case (WM_PAINT): { drawWinBmap(hwnd, &bmap2); }
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {
	// Genertation Step

	seedRandGen();
	genImages();

	// Event Loop

	createWindow(wndProc, "Noise");
	eventLoop();

	// Cleanup Step

	cleanup();

	return 0;
}