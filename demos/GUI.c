
#define RASTERON_WIN_HEIGHT 256

#include "Util_OS.h"

#include "Experimental.h"

#define MQUEUE_COUNT 16
// #define MQUEUE_COUNT 14

Rasteron_Queue* masterQueue;

unsigned long elapseSecs = 0;

#ifdef _WIN32

BITMAP bmap;

void CALLBACK timerCallback(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime){ elapseSecs++; }

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    RECT rect;

    switch(message){
        case (WM_CREATE): { 
            SetTimer(hwnd, 0, 1000, &timerCallback);
            bmap = createWinBmap(getFrameAt(masterQueue, 0));
        }
        case (WM_PAINT): { drawWinBmap(hwnd, &bmap); }
	    case (WM_CLOSE): {}
        case (WM_TIMER): {
            GetClientRect(hwnd, &rect);
            InvalidateRect(hwnd, &rect, FALSE);

            masterQueue->index = elapseSecs % MQUEUE_COUNT;
            // printf("Elapse secs: %d, GUI index: %d \n", elapseSecs, masterQueue->index);
            bmap = createWinBmap(getFrameAt(masterQueue, masterQueue->index));
        }
        default: return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

#endif

int main(int argc, char** argv) {
    masterQueue = alloc_queue("master", (ImageSize){ 256, 256}, MQUEUE_COUNT);
    
    Rasteron_Queue* mQueue_icon = loadUI_iconBtn(MENU_Large, "zoom_out");
    Rasteron_Queue* mQueue_button = loadUI_checkBtn(MENU_Large);
    Rasteron_Queue* mQueue_dial = loadUI_dial(MENU_Large, 4);
    Rasteron_Queue* mQueue_slider = loadUI_slider(MENU_Large, 6);
    // Rasteron_Queue* mQueue_slider = loadUI_slider(MENU_Large, 7);

    addFrameAt(masterQueue, getFrameAt(mQueue_icon, 0), 0);
    addFrameAt(masterQueue, getFrameAt(mQueue_icon, 1), 1);
    addFrameAt(masterQueue, getFrameAt(mQueue_icon, 2), 2);
    addFrameAt(masterQueue, getFrameAt(mQueue_button, 0), 3);
    addFrameAt(masterQueue, getFrameAt(mQueue_button, 1), 4);
    addFrameAt(masterQueue, getFrameAt(mQueue_button, 2), 5);
    for(unsigned t = 0; t < 4; t++) addFrameAt(masterQueue, getFrameAt(mQueue_dial, t), t + 6);
    for(unsigned l = 0; l < 6; l++) addFrameAt(masterQueue, getFrameAt(mQueue_slider, l), l + 10);
#ifdef _WIN32
    createWindow(wndProc, "GUI", 210, 225);
	eventLoop(NULL);
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "GUI", 256, 256);

    XImage* bmap = createUnixBmap(&platformContext, getFrameAt(masterQueue, 0));
    eventLoop(platformContext.display, NULL);
#endif

    // dealloc_image(bgImg);
    // dealloc_image(menuImg1); dealloc_image(menuImg2);

    dealloc_queue(masterQueue);
    dealloc_queue(mQueue_button); dealloc_queue(mQueue_dial); dealloc_queue(mQueue_slider);

    return 0;
}