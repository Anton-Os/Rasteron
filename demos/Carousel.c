
#include "Util_OS.h"

#define RASTERON_ENABLE_ANIM
// #define RASTERON_ENABLE_FONT

Rasteron_Queue* playQueue; // Global variable for playing back from queue

#include "Rasteron.h"

#include "catalouge/Catalouge.h"

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message){
        case (WM_CREATE): { /* bmap = createWinBmap(getFrameAt(playQueue, playQueue->state)); */ }
        case (WM_PAINT): { /* drawWinBmap(hwnd, &bmap); */ }
	    case (WM_CLOSE): {}
        default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

#endif

int main(int argc, char** argv) {
    playQueue = alloc_queue("playQueue", (ImageSize){ 1024, 1024}, 60);

#ifdef _WIN32
    createWindow(wndProc, "Canvas");
	eventLoop();
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "Canvas");

    XImage* bmap = createUnixBmap(&platformContext, canvasImg);
    XEvent event;
	while(1) {
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}
#endif

    dealloc_queue(playQueue);

    return 0;
}