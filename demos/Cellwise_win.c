#include "Toolbox.h"
#include "Cellwise.h"

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
		// loadImage_BMP("C:\\AntonDocs\\Design\\PurpleCult2.bmp", &img2);
		loadImage_BMP("C:\\AntonDocs\\Design\\Small.bmp", &img2);

		NebrTable_List* neighborTable = gen_nebrTables(
			img2.imageData.bmp.data, 
			abs(img2.imageData.bmp.width), // abs is bmp specific
			abs(img2.imageData.bmp.height) // abs is bmp specific
		);
		print_nebrTables(neighborTable);
		del_nebrTables(neighborTable);

        // Cellwise modifier functions

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