#include "Toolbox.h"
#include "Cellwise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Image image = { 0 };
NebrTable_List* neighborTable;
BITMAP winBmap;

void cleanup() {
	delNebrTables(neighborTable);
	delFileImage_BMP(&image);
}

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
	createWindow(wndProc, "Cellwise");
	eventLoop();

	cleanup(); // cleanup step

	return 0;
}