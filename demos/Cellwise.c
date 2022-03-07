#define RASTERON_ENABLE_PLUGIN
#include "Rasteron.h"

// Global Definitions
Rasteron_Image* blankImg;
Rasteron_Image* seededImg;
Rasteron_Image* scanPatImg;
Rasteron_Image* fPatImg;
Rasteron_Image* ePatImg;

// Start with simple black and white patterns
unsigned callback2(unsigned right, unsigned left) {
	if (right == BLACK_COLOR) return WHITE_COLOR;
	else return BLACK_COLOR;
}

unsigned callback4(unsigned bottom, unsigned right, unsigned left, unsigned top) {
	return ZERO_COLOR; // for testing
}

unsigned callback8(unsigned br, unsigned b, unsigned bl, unsigned r, unsigned l, unsigned tr, unsigned t, unsigned tl) {
	return ZERO_COLOR; // for testing
}

void genImages() {
	blankImg = createImgBlank(1200, 1000, BLACK_COLOR);
	seededImg = createImgSeedRaw(blankImg, WHITE_COLOR, 0.05);

	scanPatImg = createScanPatImg(seededImg, callback2);
	fPatImg = createCellPatImg4(seededImg, callback4);
	ePatImg = createCellPatImg8(seededImg, callback8);
}

void cleanup() {
	deleteImg(blankImg);
	deleteImg(seededImg);

	deleteImg(scanPatImg);
	deleteImg(fPatImg);
	deleteImg(ePatImg);
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
		bmap = createWinBmap(fPatImg);
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