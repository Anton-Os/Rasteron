#include "Toolbox.h"
#include "Rasteron.h"
#include "Cellwise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Rasteron_Image* blankImg;
Rasteron_Image* scatterImg;
Rasteron_Image* fourPatternImg;
Rasteron_Image* eightPatternImg;

unsigned callback4(unsigned bottom, unsigned right, unsigned left, unsigned top) {
	return 1; // for testing
}

unsigned callback8(unsigned br, unsigned b, unsigned bl, unsigned r, unsigned l, unsigned tr, unsigned t, unsigned tl) {
	return 1; // for testing
}

void genImages() {
	blankImg = createImgBlank(1200, 1000, 0xFF000000);
	scatterImg = createImgScatter(blankImg, 0xFFFFFFFF, 0.05);

	// fourPatternImg = createPatternImg4(scatterImg, callback4);
	fourPatternImg = createPatternImg4(scatterImg, callback4);
	eightPatternImg = createPatternImg8(scatterImg, callback8);
}

void cleanup() {
	deleteImg(blankImg);
	deleteImg(scatterImg);
	deleteImg(fourPatternImg);
	deleteImg(eightPatternImg);
}

#ifdef _WIN32

BITMAP winBmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		genImages();
		// winBmap = createWinBmap(&image);
	}
	case (WM_PAINT): {
        // Implement Draw Call
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
	genImages;
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