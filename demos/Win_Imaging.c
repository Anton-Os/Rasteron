#include "Toolbox.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Image img1 = { 0 };
BITMAP winBmap1;
Image img2 = { 0 };
BITMAP winBmap2;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		loadImage_TIFF("C:\\AntonDocs\\Design\\PurpleCult.tif", &img1);
        winBmap1 = createWinBmap(&img1);
		loadImage_BMP("C:\\AntonDocs\\Design\\PurpleCult2.bmp", &img2);
		winBmap2 = createWinBmap(&img2);
	}
	case (WM_PAINT): {
        drawWinBmap(hwnd, &winBmap1);    
	}
	case (WM_DESTROY): {  }
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

	delImage_TIFF(&img1);
	delImage_BMP(&img2);

	return 0;
}