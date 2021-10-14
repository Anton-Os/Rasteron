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
Rasteron_Image* splatterImg;

void genImages(){
	blankImg = createImgBlank(1200, 1000, 0xFF73e5ff);
	// randNoiseImg = createRandNoiseImg(0xFFFFFF00, 0xFF73e5ff, blankImg);
	randNoiseImg = createRandNoiseImg(0xFFFFFFFF, 0xFF000000, blankImg);
	randNoiseImg2 = createRandNoiseImg(0xFFFFFFFF, 0xFF00FFFF, blankImg);
	scatterImg = createImgScatter(randNoiseImg2, 0xFFFF00FF, 0.1);
}

void cleanup() {
	deleteImg(blankImg);
	deleteImg(randNoiseImg);
	deleteImg(randNoiseImg2);
	deleteImg(scatterImg);
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

		bmap = createWinBmapRaw(scatterImg->width, scatterImg->height, scatterImg->data);
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