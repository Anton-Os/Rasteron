
#include "Util_OS.h"

#include "experimental/Experimental.h"

#define ROULETTE_COUNT 12

Rasteron_Queue* roulette;

unsigned long elapseSecs = 0;

#ifdef _WIN32

BITMAP bmap;

void CALLBACK timerCallback(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime){ elapseSecs++; }


LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    RECT rect;

    switch(message){
        case (WM_CREATE): { 
            SetTimer(hwnd, 0, 1000, &timerCallback);
            bmap = createWinBmap(getFrameAt(roulette, 0));
        }
        case (WM_PAINT): { drawWinBmap(hwnd, &bmap); }
	    case (WM_CLOSE): {}
        case (WM_TIMER): {
            GetClientRect(hwnd, &rect);
            InvalidateRect(hwnd, &rect, FALSE);

            roulette->index = elapseSecs % ROULETTE_COUNT;
            // printf("Elapse secs: %d, Roulette index: %d \n", elapseSecs, roulette->index);
            bmap = createWinBmap(getFrameAt(roulette, roulette->index));
        }
        default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

#endif

int main(int argc, char** argv) {
    roulette = alloc_queue("roulette", (ImageSize){ 1024, 1024}, ROULETTE_COUNT);
    
    Rasteron_Queue* menuQueue1 = loadUI_checkBtn(MENU_Tiny);
    Rasteron_Queue* menuQueue2 = loadUI_dial(MENU_Tiny, 8);
    Rasteron_Queue* menuQueue3 = loadUI_slider(MENU_Tiny, 2);
    // Rasteron_Image* bgImg = solidImgOp((ImageSize){ 1024, 1024 }, DARK_COLOR);
    // Rasteron_Image* menuImg1 = insertImgOp(getFrameAt(menuQueue1, 0), bgImg, 0.0, 0.0);
    // Rasteron_Image* menuImg2 = insertImgOp(getFrameAt(menuQueue1, 1), bgImg, 0.0, 0.0);

    addFrameAt(roulette, getFrameAt(menuQueue1, 0), 0);
    addFrameAt(roulette, getFrameAt(menuQueue1, 1), 1);
    for(unsigned t = 0; t < 8; t++) addFrameAt(roulette, getFrameAt(menuQueue2, t), t + 2);
    addFrameAt(roulette, getFrameAt(menuQueue3, 0), 10);
    addFrameAt(roulette, getFrameAt(menuQueue3, 1), 11);
#ifdef _WIN32
    createWindow(wndProc, "Roulette");
	eventLoop();
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "Roulette");

    XImage* bmap = createUnixBmap(&platformContext, getFrameAt(roulette, 0));
    XEvent event;
	while(1) {
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}
#endif

    // dealloc_image(bgImg);
    // dealloc_image(menuImg1); dealloc_image(menuImg2);

    dealloc_queue(roulette);
    // dealloc_queue(menuQueue1); // dealloc_queue(menuQueue2); // dealloc_queue(menuQueue3);

    return 0;
}