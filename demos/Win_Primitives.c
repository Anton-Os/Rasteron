#include "Rasteron.h"
#include "Heightmap.h"

#include <stdio.h>
#include <stdlib.h>

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
	}
	case (WM_PAINT): {
		Image img2 = { 0 };
		rstnLoadFromFile("C:\\AntonDocs\\Design\\PurpleCult.png", &img2);
		// loadImage_TIFF("C:\\AntonDocs\\Design\\PurpleCult.tif", &img2);

		Rasteron_Image* image = createImgBase(&img2);
		Rasteron_Palette* palette = createPalette(image);
		Rasteron_Sprite* sprite = createSprite(image);

		Rasteron_Image* imageGrey = createImgGrey(image);
		Rasteron_Heightmap* heightmap = createHeightmap(imageGrey); // Heightmap data test
		Rasteron_Palette* filtered = filterPalette(1024, palette);

		BITMAP bmapTiff = createWinBmap(&img2);
		BITMAP bmapBase = createWinBmapRaw(image->width, image->height, image->data);
		// BITMAP bmapBase = createWinBmapRaw(imageGrey->width, imageGrey->height, imageGrey->data);
		drawWinBmap(hwnd, &bmapBase);

		deleteSprite(sprite);
		deleteImg(image);
		deleteImg(imageGrey);
		deleteHeightmap(heightmap);
		deletePalette(palette);
		deletePalette(filtered);
		
		rstnDelFromFile(&img2);
	}
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {

	WNDCLASS wndClass = { 0 };
	// wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.hInstance = GetModuleHandle(NULL);
	wndClass.lpfnWndProc = wndProc;
	wndClass.lpszClassName = "Default Class";
	RegisterClass(&wndClass);

	HWND wndWindow = CreateWindow(
		"Default Class",
		"Team Purple",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 1200, 1100,
		NULL, NULL, GetModuleHandle(NULL), NULL
	);

	ShowWindow(wndWindow, 1);
	UpdateWindow(wndWindow);

	MSG wndMessage;
	BOOL bRet;


	return 0;
}