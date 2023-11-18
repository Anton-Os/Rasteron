
#include "Util_OS.h"

#define RASTERON_ENABLE_ANIM
// #define RASTERON_ENABLE_FONT

#include "Rasteron.h"

#include "catalouge/Catalouge.h"



#define CAROUSEL_COUNT 10

Rasteron_Queue* carousel;
Rasteron_Image* stageImg;

unsigned long elapseSecs = 0;

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    RECT rect;

    switch(message){
        case (WM_CREATE): { 
            carousel = alloc_queue("carousel", (ImageSize){ 1024, 1024}, CAROUSEL_COUNT);
            stageImg = solidImgOp((ImageSize){ 1024, 1024}, 0xFF00FF00);

            bmap = createWinBmap(stageImg);
        }
        case (WM_PAINT): { drawWinBmap(hwnd, &bmap); }
	    case (WM_CLOSE): {}
        case (WM_TIMER): {
            GetClientRect(hwnd, &rect);
            InvalidateRect(hwnd, &rect, FALSE);

            elapseSecs++; 
            carousel->index = elapseSecs % CAROUSEL_COUNT;
            // printf("Elapse secs: %d, Carousel index: %d \n", elapseSecs, carousel->index);
            bmap = createWinBmap(getFrameAt(carousel, carousel->index));
        }
        default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

#endif

int main(int argc, char** argv) {
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

    dealloc_queue(carousel);
    dealloc_image(stageImg);

    return 0;
}