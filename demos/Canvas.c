#include "Util_OS.h"

#include "Rasteron.h"

#include "catalouge/Catalouge.h"

Rasteron_Image* canvasImg;

enum CANVAS_Mode { CANVAS_Blank, 
	CANVAS_Slice, CANVAS_Nesting,
	CANVAS_Distill, CANVAS_Overlay, 
	CANVAS_MNoise, CANVAS_CAuto, 
	CANVAS_Prox, CANVAS_Fractal 
};

static double arg1 = 0.0;
static double arg2 = 0.0;

Rasteron_Image* createCanvasImg(enum CANVAS_Mode mode){
	Rasteron_Image* image;

	switch(mode){
		case CANVAS_Slice: return slicediceImgOp(FLIP_None, 0, CROP_None, 0.0);
		case CANVAS_Nesting: return supernestImgOp(arg1, arg2);
		case CANVAS_Distill: return distillingImgOp();
		case CANVAS_Overlay: return overlayerImgOp();
		case CANVAS_MNoise: return multiNoiseImgOp();
		case CANVAS_CAuto: return cellAutomataImgOp(); 
		case CANVAS_Prox: return proxPatternImgOp();
		case CANVAS_Fractal: return fractalsImgOp();
		default: return solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00);
	}

	return image;
}

#ifdef _WIN32

BITMAP bmap;

static char keysave = '\0';

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	POINT point;
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		canvasImg = createCanvasImg(CANVAS_Slice);
		bmap = createWinBmap(canvasImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		if(isalnum(wParam)) {
			dealloc_image(canvasImg);
			if(isalpha(wParam)) keysave = (char)tolower(wParam);
		}

		switch((char)wParam){
			case '0': arg1 = 0.0; arg2 = 0.0; break; // reset arguments
			case '1': arg1 = 1.0; break; case '2': arg2 = 1.0; break;
			case '3': arg1 = -1.0; break; case '4': arg2 = -1.0; break;
			case '5': arg1 += 0.05; break; case '6': arg2 += 0.05; break;
			case '7': arg1 -= 0.05; break; case '8': arg2 -= 0.05; break;
			case '9': arg1 = -1.0 + (((double)rand()) / RAND_MAX); // randomize arg1
					  arg2 = -1.0 + (((double)rand()) / RAND_MAX); // randomize arg2
					  break;
		}

		switch(keysave){
			case 'a': canvasImg = createCanvasImg(CANVAS_Slice); break;
			case 'b': canvasImg = createCanvasImg(CANVAS_Nesting); break;
			case 'c': canvasImg = createCanvasImg(CANVAS_Distill); break;
			case 'd': canvasImg = createCanvasImg(CANVAS_Overlay); break;
			case 'e': canvasImg = createCanvasImg(CANVAS_MNoise); break;
			case 'f': canvasImg = createCanvasImg(CANVAS_CAuto); break;
			case 'g': canvasImg = createCanvasImg(CANVAS_Prox); break;
			case 'h': canvasImg = createCanvasImg(CANVAS_Fractal); break;
			default: canvasImg = createCanvasImg(CANVAS_Blank); break;
		}

		putchar(wParam);
	
		GetClientRect(hwnd, &rect);
		InvalidateRect(hwnd, &rect, FALSE);

		bmap = createWinBmap(canvasImg);
	}}
	case (WM_PAINT): { drawWinBmap(hwnd, &bmap); }
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#endif

int main(int argc, char** argv) {
	srand(time(NULL));

	puts("Please refer to following commands to select images for canvas:\n");
	puts("\n Use numbers 0 to 9 to modify function arguments:\n");
	puts("\t Reset arguments with 0 \n");
	puts("\t Odd numbers set or increment argument 1 in positive or negative \n");
	puts("\t Even numbers set or increment argument 2 in positive or negative \n");
	puts("\t Randomize arguments (range -1 to 1) with 9 \n");

	puts("\n Alphabetical characters A to H output images dedicated to various Rasteron API functionalities. Refer to Catalouge.h for output examples \n");
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

	if(canvasImg != NULL)
		writeFileImageRaw("Output", IMG_Png, canvasImg->height, canvasImg->width, canvasImg->data);

    dealloc_image(canvasImg); // cleanup
    return 0;
}