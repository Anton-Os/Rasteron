#include "Toolbox.h"
#include "Cellwise.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
Image image = { 0 };
NebrTable_List* neighborTable;

void init(){
	loadFileImage_BMP("C:\\AntonDocs\\Codex\\Ao-Project\\Rasteron\\master\\assets\\Logo.bmp", &image); // hard path
	neighborTable = genNebrTables(
		image.imageData.bmp.data, 
		abs(image.imageData.bmp.width), // abs is bmp specific
		abs(image.imageData.bmp.height) // abs is bmp specific
	);

	printNebrTables(neighborTable);
}

void cleanup() {
	delNebrTables(neighborTable);
	delFileImage_BMP(&image);
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