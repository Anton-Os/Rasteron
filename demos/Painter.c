#include "Util_OS.h"

#include "Experimental.h"

unsigned checkerXY(double x, double y){
    unsigned c = x * 2;
	unsigned r = y * 2;

	if(c % 2 == 0 && r % 2 == 0) return 0xFF111111;
	else if(c % 2 == 0 && r % 2 == 1) return 0xFF111166; 
	else if(c % 1 == 0 && r % 2 == 0) return 0xFF116611; 
	else return 0xFF661111; 
}

Rasteron_Image* canvasImg;
char inputStr[1024];

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		canvasImg = mapImgOp((ImageSize){1024, 1024}, checkerXY);
		bmap = createWinBmap(canvasImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, FALSE);

		bmap = createWinBmap(canvasImg);
	}}
	case (WM_PAINT): { 
		InvalidateRect(hwnd, &rect, TRUE);
		drawWinBmap(hwnd, &bmap); 
	}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#endif

void inputCallback(){
	puts("Enter a command: ");
	scanf("%s", &inputStr);
	
	if(inputStr[0] == '0' && inputStr[1] == 'x') puts("Solid color detected");
	// TODO: Detect if file path was set to png, tiff, or bmp
	// TODO: Detect procedural operations
	else puts("Unknown operation!");
}

int main(int argc, char** argv){
    canvasImg = mapImgOp((ImageSize){1024, 1024}, checkerXY); // global canvas for drawing

#ifdef _WIN32
    createWindow(wndProc, "Painter", 1024, 1024);
	eventLoop(inputCallback);
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "Painter", 1024, 1024);

    XImage* bmap = createUnixBmap(&platformContext, canvasImg);
    eventLoop(platformContext.display, inputCallback);
#endif

    dealloc_image(canvasImg); // cleanup
    return 0;
}