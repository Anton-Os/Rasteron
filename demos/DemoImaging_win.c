#include "ImageLoader.h"

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
		//Image img1 = { 0 };
		/* loadImage_TIFF("C:\\AntonDocs\\Design\\PurpleCult.tif", &img1);
		makeSolidColor(img1.imageData.tiff.raster,
			img1.imageData.tiff.length * img1.imageData.tiff.width,
			0x00FFFF00); */
        //BITMAP bmap1 = createWinBmap(&img1);
        //drawWinBmap(hwnd, &bmap1);
		//delImage_TIFF(&img1); // FIX THIS!!! Unresolved?

		Image img2 = { 0 };
		loadImage_BMP("\\..\\res\\PurpleCult.bmp", &img2);
		BITMAP bmap2 = createWinBmap(&img2);
        drawWinBmap(hwnd, &bmap2);
        delImage_BMP(&img2);
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