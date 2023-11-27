#include "Util_OS.h"

#include "experimental/Experimental.h"

Rasteron_Image* canvasImg; // Global variable for drawing onto canvas

enum CANVAS_Mode { CANVAS_Blank, 
	CANVAS_Slice, CANVAS_Nesting,
	CANVAS_Distill, CANVAS_Overlay, 
	CANVAS_MNoise, CANVAS_CAuto, 
	CANVAS_Prox, CANVAS_Text 
};

#define CANVAS_PRESET_MIN -1
#define CANVAS_PRESET_MAX 2

static int pArg = -1;
static double xArg = 0.0;
static double yArg = 0.0;

static Rasteron_Image* createCanvasImg(enum CANVAS_Mode mode){
	Rasteron_Image* image;

	switch(mode){
		case CANVAS_Slice: return slicediceImgOp(pArg, xArg, yArg);
		case CANVAS_Nesting: return nestboxesImgOp(xArg, yArg);
		case CANVAS_Distill: return distillingImgOp(pArg);
		case CANVAS_Overlay: return overlayerImgOp(pArg, 0xFFFF0000, 0xFF00FF00);
		case CANVAS_Text: return dynamicTextImgOp("Hello World");
		case CANVAS_MNoise: return multiNoiseImgOp();
		case CANVAS_CAuto: return cellAutomataImgOp(); 
		case CANVAS_Prox: return proxPatternImgOp();
		default: return NULL; // solidImgOp((ImageSize){ 1024, 1024 }, 0xFFFFFF00);
	}

	return image;
}

#ifdef _WIN32

BITMAP bmap;

static char keysave = 'd';
static char* keyseq = "Hello World";
static char textBuffer[1024];

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	POINT point;
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		canvasImg = createCanvasImg(CANVAS_Overlay);
		bmap = createWinBmap(canvasImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		if(isalnum(wParam)) {
			if(canvasImg != NULL) dealloc_image(canvasImg);
			if(isalpha(wParam)) keysave = (char)tolower(wParam);
		}

		switch((char)wParam){
			case '0': xArg = 0.0; yArg = 0.0; pArg = 0; break;
			case '1': pArg++; break; case '3': pArg--; break;
			case '8': yArg += 0.05F; break; case '2': yArg -= 0.05F; break;
			case '6': xArg += 0.05F; break; case '4': xArg -= 0.05F; break;
			case '7': double temp = xArg; xArg = yArg; yArg = temp; break; // flip arguments 1 and 2
			case '9': xArg = (rand() / (double)(RAND_MAX)) * 2 - 1; // randomize arg 1
					  yArg = (rand() / (double)(RAND_MAX)) * 2 - 1; // randomize arg 2
		}
		if(pArg > CANVAS_PRESET_MAX) pArg = CANVAS_PRESET_MIN; // bounds pArg to upper limit
		else if(pArg < CANVAS_PRESET_MIN) pArg = CANVAS_PRESET_MAX;

		switch(keysave){
			case 'a': canvasImg = createCanvasImg(CANVAS_Slice); break;
			case 'b': canvasImg = createCanvasImg(CANVAS_Nesting); break;
			case 'c': canvasImg = createCanvasImg(CANVAS_Distill); break;
			case 'd': canvasImg = createCanvasImg(CANVAS_Overlay); break;
			case 'e': canvasImg = createCanvasImg(CANVAS_Text); break;
			case 'f': canvasImg = createCanvasImg(CANVAS_MNoise); break;
			case 'g': canvasImg = createCanvasImg(CANVAS_CAuto); break;
			case 'h': canvasImg = createCanvasImg(CANVAS_Prox); break;
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
	initFreeType();

	puts("Please refer to following commands to select images for canvas:\n");
	puts("\n Use numbers 0 to 9 to modify function arguments:\n");
	puts("\t Reset arguments with 0 \n");
	puts("\t Odd numbers set or increment argument 1 in positive or negative \n");
	puts("\t Even numbers set or increment argument 2 in positive or negative \n");
	puts("\t Randomize arguments (range -1 to 1) with 9 \n");

	puts("\n Alphabetical characters A to H output images dedicated to various Rasteron API functionalities. Refer to experimental/Experimental.h for output examples \n");
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