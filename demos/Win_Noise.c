#include "Rasteron.h"
#include "Lattice.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions

Rasteron_Image* blankImg;
Rasteron_Image* randNoiseImg;
Rasteron_Image* latticeNoiseImg;

BITMAP bmap;

void cleanup() {
	deleteImg(blankImg);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		// loadFileImage("C:\\AntonDocs\\Design\\PurpleCult.png", &img);
		blankImg = createImgBlank(400, 400, 0xFF73e5ff);

		bmap = createWinBmapRaw(blankImg->width, blankImg->height, blankImg->data);
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