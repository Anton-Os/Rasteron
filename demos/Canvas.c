#include "OS_Util.h"

#include "Rasteron.h"

Rasteron_Image* canvasImg;

Rasteron_Image* createCanvasImg(){
    Rasteron_Image* image;

	// TODO: Add body here

    return image;
}

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): { bmap = createWinBmap(canvasImg); }
	case (WM_PAINT): { drawWinBmap(hwnd, &bmap); }
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#endif

int main(int argc, char** argv) {
	canvasImg = createCanvasImg();

#ifdef _WIN32
    createWindow(wndProc, "Synthesis");
	eventLoop();
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "Synthesis");

    XImage* bmap = createUnixBmap(&platformContext, canvasImg);
    XEvent event;
	while(1) {
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}
#endif

	if(canvasImg != NULL)
		writeFileImageRaw("Output", IMG_Png, canvasImg->height, canvasImg->width, canvasImg->data);

    free_image(canvasImg); // cleanup
    return 0;
}