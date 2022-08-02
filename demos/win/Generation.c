#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
const char* imageName = "Logo.png";
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

BITMAP bmap1, bmap2, bmap3;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap2 = createWinBmap(greyImage);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap2);
	}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


int main(int argc, char** argv) {
	genFullImagePath();
	
	// Genertation Step

	sourceImage = createImgRef(fullImagePath);
	greyImage = createImgGrey(sourceImage);
	redImage = createImgFltChan(sourceImage, CHANNEL_Red);
	blueImage = createImgAvgChan(sourceImage, CHANNEL_Blue);
	sprite = createSprite(sourceImage);
	heightmap = createHeightmap(sourceImage);

	// Event Loop

	createWindow(wndProc, "Primitives");
	eventLoop();

	// Cleanup Step

	deleteImg(sourceImage);
	deleteImg(greyImage);
	deleteImg(redImage);
	deleteImg(blueImage);
	deleteSprite(sprite);
	deleteHeightmap(heightmap);

	return 0;
}