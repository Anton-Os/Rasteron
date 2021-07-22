#include "Rasteron.h"
#include "Heightmap.h"

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
Rasteron_Heightmap* heightmap;

BITMAP bmap1;
BITMAP bmap2;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		rstnLoadFromFile("C:\\AntonDocs\\Design\\PurpleCult.png", &img);

		imageBase = createImgBase(&img);
		imageGrey = createImgGrey(imageBase);
		imageRed = createImgFilter(imageBase, CHANNEL_Red);
		imageBlue = createImgFilter(imageBase, CHANNEL_Blue);
		palette = createPalette(imageBase);
		sprite = createSprite(imageBase);
		heightmap = createHeightmap(imageGrey); // Heightmap data test

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

	while (1) {
		bRet = GetMessage(&wndMessage, NULL, 0, 0);
		if (bRet > 0){  // (bRet > 0 indicates a message that must be processed.)
			TranslateMessage(&wndMessage);
			DispatchMessage(&wndMessage);
		}  // (bRet == -1 indicates an error.)
		else break;  // (bRet == 0 indicates "exit program".)
	}

	// Cleanup Step

	deleteSprite(sprite);
	deleteImg(imageBase);
	deleteImg(imageGrey);
	deleteImg(imageRed);
	deleteImg(imageBlue);
	deleteHeightmap(heightmap);
	deletePalette(palette);

	rstnDelFromFile(&img);

	return 0;
}