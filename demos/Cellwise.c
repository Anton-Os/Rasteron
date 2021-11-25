#include "Toolbox.h"
#include "Rasteron.h"
#include "Cellwise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Rasteron_Image* blankImg;
NebrTable_List* neighborTable;

void init()
	blankImg = createImgBlank(1200, 1000, 0xFF73e5ff);
	neighborTable = genNebrTables(
		blankImg->data, 
		abs(blankImg->width), // abs is bmp specific
		abs(blankImg->height) // abs is bmp specific
	);

	printNebrTables(neighborTable);
}

void cleanup() {
	deleteImg(blankImg);
	delNebrTables(neighborTable);
}

#ifdef _WIN32

BITMAP winBmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		init();
		// winBmap = createWinBmap(&image);
	}
	case (WM_PAINT): {
        // Implement Draw Call
	}
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#elif defined __linux__

void unixProc(){
	UnixContext context;
	createWindow(&context, "Cellwise");
	init();
}

#endif

int main(int argc, char** argv) {

#ifdef _WIN32
	createWindow(wndProc, "Cellwise");
#elif defined __linux__
	unixProc();
#endif
	eventLoop();

	cleanup(); // cleanup step

	return 0;
}