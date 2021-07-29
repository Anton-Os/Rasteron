#include "Noise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions

Rasteron_Image* blankImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* randNoiseImg2;
Rasteron_Image* latticeNoiseImg;

BITMAP bmap;

void cleanup() {
	deleteImg(blankImg);
	deleteImg(randNoiseImg);
	deleteImg(randNoiseImg2);
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
		randNoiseImg2 = createRandNoiseImg(0xFFFFFFFF, 0xFF0000FF, blankImg);

		// bmap = createWinBmapRaw(blankImg->width, blankImg->height, blankImg->data);
		bmap = createWinBmapRaw(randNoiseImg2->width, randNoiseImg2->height, randNoiseImg2->data);
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