#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

// #define ANIM_HEIGHT 1100
// #define ANIM_WIDTH 1200

unsigned long ticker = 0;
Rasteron_Animation* animation = NULL;
// Rasteron_Image* frame1;
Rasteron_Image* frame2;
Rasteron_Image* frame3;
Rasteron_Image* frame4;
Rasteron_Image* compositeImg = NULL;

void genImages() {
	// frame1 = createSolidImg((ImageSize){ ANIM_HEIGHT, ANIM_WIDTH }, 0xFF000000);
    frame2 = createSolidImg((ImageSize){ 256, 2000 }, 0xFFFF0000);
    frame3 = createSolidImg((ImageSize){ 2000, 256 }, 0xFF00FF00);
    frame4 = createSolidImg((ImageSize){ 2000, 2000 }, 0xFF0000FF);

	animation = alloc_animation("sequence", 4);
    // addFrameAt(animation, frame1, 0);
    addFrameAt(animation, frame2, 1);
    addFrameAt(animation, frame3, 2);
    addFrameAt(animation, frame4, 3);

	// if(animation == NULL) puts("animation is null");
	compositeImg = compositeImg(animation);
}

void cleanup() {
	free_animation(animation);
	// free_image(frame1);
	free_image(frame2);
	free_image(frame3);
	free_image(frame4);
	free_image(compositeImg);
}

BITMAP bmap1, bmap2, bmap3, bmap4;
BITMAP* bmap_active;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap1 = createWinBmap(getFrame(animation, 0));
		bmap2 = createWinBmap(getFrame(animation, 1));
		bmap3 = createWinBmap(getFrame(animation, 2));
		bmap4 = createWinBmap(getFrame(animation, 3));
	}
	case (WM_PAINT): {
		switch (ticker % 8) { // Extending duration per Frame
		case 0: case 1: drawWinBmap(hwnd, &bmap4); break;
		case 2: case 3: drawWinBmap(hwnd, &bmap3); break;
		case 4: case 5: drawWinBmap(hwnd, &bmap2); break;
		case 6: case 7: drawWinBmap(hwnd, &bmap1); break;
		}
	}
	case (WM_TIMER): {
		ticker++; // increment the ticker

		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, FALSE);
	}
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {
	// Generation Step
	
	genImages();

	// Event Loop

	createWindow(wndProc, "Sequence");
	eventLoop();

	// Cleanup Step

	cleanup();

	return 0;
}