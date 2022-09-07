#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
const char* imageName = "Logo.bmp";
char fullImagePath[1024];

Rasteron_Image* sourceImage;
Rasteron_Image* greyImage;
Rasteron_Image* redImage;
Rasteron_Image* blueImage;
Rasteron_Sprite* sprite;
Rasteron_Heightmap* heightmap;

// TODO: Move this to Toolbox or OS_Util
void genFullImagePath(const char* name) {
	strcpy(fullImagePath, IMAGE_DIR);
	strcat(fullImagePath, "\\");
	strcat(fullImagePath, name);	
	fixPathDashes(&fullImagePath);
}

void genImages(){
	sourceImage = createRefImg(fullImagePath);
	greyImage = createGreyImg(sourceImage);
	redImage = createFltChanImg(sourceImage, CHANNEL_Red);
	blueImage = createAvgChanImg(sourceImage, CHANNEL_Blue);
	sprite = createSprite(sourceImage);
	heightmap = createHeightmap(sourceImage);
}

void cleanup(){
	deleteImg(sourceImage);
	deleteImg(greyImage);
	deleteImg(redImage);
	deleteImg(blueImage);
	deleteSprite(sprite);
	deleteHeightmap(heightmap);
}

BITMAP bmap1, bmap2, bmap3;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap1 = createWinBmap(greyImage);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap1);
	}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


int main(int argc, char** argv) {
	genFullImagePath(imageName);
	
	// Genertation Step

	genImages();

	// Event Loop

	createWindow(wndProc, "Generation");
	eventLoop();

	// Cleanup Step

	cleanup();

	return 0;
}