#include "Rasteron.h"
#include "Lattice.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Image img = { 0 };
Rasteron_Image* imageBase;
Rasteron_Image* imageGrey;
Rasteron_Image* imageRed;
Rasteron_Image* imageBlue;
Rasteron_Palette* palette;
Rasteron_Sprite* sprite;
Rasteron_Lattice* heightmap;

BITMAP bmap1;
BITMAP bmap2;

void cleanup() {
	deleteSprite(sprite);
	deleteImg(imageBase);
	deleteImg(imageGrey);
	deleteImg(imageRed);
	deleteImg(imageBlue);
	deleteLattice(heightmap);
	deletePalette(palette);

	delFileImage(&img);
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		loadFileImage("C:\\AntonDocs\\Design\\PurpleCult.png", &img);

		imageBase = createImgBase(&img);
		imageGrey = createImgGrey(imageBase);
		imageRed = createImgFilter(imageBase, CHANNEL_Red);
		imageBlue = createImgFilter(imageBase, CHANNEL_Blue);
		palette = createPalette(imageBase);
		sprite = createSprite(imageBase);
		heightmap = createLattice(imageGrey); // Lattice data test

		bmap1 = createWinBmap(&img);
		bmap2 = createWinBmapRaw(imageRed->width, imageRed->height, imageRed->data);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap2);
	}
	case (WM_CLOSE): {
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {

	createWindow(wndProc, "Primitives");
	eventLoop();

	cleanup(); // cleanup step

	return 0;
}