#include "Util_OS.h"

#include "Experimental.h"

Rasteron_Image* canvasImg; // Global variable for drawing onto canvas

#define CANVAS_PRESET_MIN -1
#define CANVAS_PRESET_MAX 2

static int pArg = -1;
static double xArg = 0.0;
static double yArg = 0.0;

static char keysave = 'd';
static char* keyseq = "Hello World";
static char textBuffer[1024];

void setupCanvas(char input){
	if(isalnum(input)) {
		if(canvasImg != NULL) dealloc_image(canvasImg);
		if(isalpha(input)) keysave = (char)tolower(input);
	}

	switch(input){
		case '0': xArg = 0.0; yArg = 0.0; pArg = 0; break;
		case '1': pArg++; break; case '3': pArg--; break;
		case '8': yArg += 0.05F; break; case '2': yArg -= 0.05F; break;
		case '6': xArg += 0.05F; break; case '4': xArg -= 0.05F; break;
		case '7': double temp = xArg; xArg = yArg; yArg = temp; break; // flip arguments 1 and 2
		case '9': xArg = ((double)rand() / (RAND_MAX / 2.0)) - 1.0;
				  yArg = ((double)rand() / (RAND_MAX / 2.0)) - 1.0;
	}
	if(pArg > CANVAS_PRESET_MAX) pArg = CANVAS_PRESET_MIN; // bounds pArg to upper limit
	else if(pArg < CANVAS_PRESET_MIN) pArg = CANVAS_PRESET_MAX;

	switch(keysave){
		case 'a': canvasImg = oragamiImgOp(pArg, xArg, yArg); break;
		case 'b': canvasImg = nestboxesImgOp(xArg, yArg); break;
		case 'c': canvasImg = distillingImgOp(pArg); break;
		case 'd': canvasImg = overlayerImgOp(pArg, 0xFFFF0000, 0xFF00FFFF); break;
		case 'e': canvasImg = wordsmithImgOp("Rasteron is Dope!!!"); break;
		case 'f': canvasImg = multiNoiseImgOp(pArg); break;
		case 'g': canvasImg = cellAutomataImgOp(pArg); break;
		case 'h': canvasImg = proxPatternImgOp((pArg + 2) * 5); break;
		default: canvasImg = solidImgOp((ImageSize){ 1024, 1024 }, RAND_COLOR()); break;
	}
}

#ifdef _WIN32

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	POINT point;
	RECT rect;

	switch (message) {
	case (WM_CREATE): { 
		canvasImg = proxPatternImgOp(20);
		bmap = createWinBmap(canvasImg); 
	}
	case (WM_CHAR): { if(wParam != 0){
		setupCanvas((char)wParam);

		putchar(wParam);
	
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

int main(int argc, char** argv) {
	srand(time(NULL));
	// initFreeType();

	puts("Please refer to following commands to select images for canvas:\n");
	puts("\n Use numbers 0 to 9 to modify function arguments:\n");
	puts("\t Reset arguments with 0 \n");
	puts("\t Odd numbers set or increment argument 1 in positive or negative \n");
	puts("\t Even numbers set or increment argument 2 in positive or negative \n");
	puts("\t Randomize arguments (range -1 to 1) with 9 \n");

	puts("\n Alphabetical characters A to H output images dedicated to various Rasteron API functionalities. Refer to experimental/Experimental.h for output examples \n");
#ifdef _WIN32
    createWindow(wndProc, "Canvas", 1024, 1024);
	eventLoop(NULL);
#elif defined __linux__
    Platform_Context platformContext;
    createWindow(&platformContext, "Canvas", 1024, 1024);

    XImage* bmap = createUnixBmap(&platformContext, canvasImg);
    eventLoop(platformContext.display, NULL);
#endif

	if(canvasImg != NULL)
		writeFileImageRaw("Output", IMG_Png, canvasImg->height, canvasImg->width, canvasImg->data);

    dealloc_image(canvasImg); // cleanup
    return 0;
}