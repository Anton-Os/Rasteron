#include "Util_OS.h"

#include "Experimental.h"

unsigned setupImgOp(double x, double y){
    // return ((x < 0.5 && y > 0.5) || (x > 0.5 && y < 0.5))? 0xFF333333 : 0xFFEEEEEE;
    return 0xFFFFFF00;
}

Rasteron_Image* canvasImg;

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		canvasImg = proxPatternImgOp(20);
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

int main(int argc, char** argv){
    canvasImg = mapImgOp((ImageSize){1024, 1024}, setupImgOp); // global canvas for drawing

    // TODO: Get input and draw over canvas

#ifdef _WIN32
    createWindow(wndProc, "Painter", 1024, 1024);
	eventLoop(NULL);
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "Painter", 1024, 1024);

    XImage* bmap = createUnixBmap(&platformContext, canvasImg);
    eventLoop(platformContext.display, NULL);
#endif

    dealloc_image(canvasImg); // cleanup
    return 0;
}