#include "Toolbox.h"
#include "Rasteron.h"
#include "Animation.h"

#include <stdio.h>
#include <stdlib.h>

// Global Definitions
#define ANIM_HEIGHT 600
#define ANIM_WIDTH 500

Rasteron_Animation* animation;
Rasteron_Image* frame1;
Rasteron_Image* frame2;
Rasteron_Image* frame3;
Rasteron_Image* frame4;

void genImages() {
	frame1 = createImgBlank(ANIM_HEIGHT, ANIM_WIDTH, 0xFF111111);
    frame2 = createImgBlank(ANIM_HEIGHT, ANIM_WIDTH, 0xFFFF0000);
    frame3 = createImgBlank(ANIM_HEIGHT, ANIM_WIDTH, 0xFF00FF00);
    frame4 = createImgBlank(ANIM_HEIGHT, ANIM_WIDTH, 0xFF0000FF);

    animation = allocNewAnim("sequence", ANIM_HEIGHT, ANIM_WIDTH, 4);
    addFrameData(animation, frame1, 0);
    addFrameData(animation, frame2, 1);
    addFrameData(animation, frame3, 2);
    addFrameData(animation, frame4, 3);
}

void cleanup() {
	deleteAnim(animation);
	deleteImg(frame1);
	deleteImg(frame2);
	deleteImg(frame3);
	deleteImg(frame4);
}

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		genImages();
		bmap = createWinBmap(getFrame(animation, 3));
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap);
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
	genImages();
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