#include "Toolbox.h"
#include "Cellwise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Image image = { 0 };
NebrTable_List* neighborTable;
BITMAP winBmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		loadFileImage_BMP("C:\\AntonDocs\\Design\\Small.bmp", &image);
		neighborTable = genNebrTables(
			image.imageData.bmp.data, 
			abs(image.imageData.bmp.width), // abs is bmp specific
			abs(image.imageData.bmp.height) // abs is bmp specific
		);
		winBmap = createWinBmap(&image);

		printNebrTables(neighborTable);
	}
	case (WM_PAINT): {
        drawWinBmap(hwnd, &winBmap);
	}
	case (WM_DESTROY): { }
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
	wndClass.lpszClassName = "Rasteron";
	RegisterClass(&wndClass);

	HWND wndWindow = CreateWindow(
		"Rasteron",
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

	delNebrTables(neighborTable);
	delFileImage_BMP(&image);

	return 0;
}