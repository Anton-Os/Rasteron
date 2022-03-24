#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
const char* imagePath = IMAGE_DIR;
const char* imageName = "Logo.png";
char targetImagePath[1024];

// Image img = { 0 };
Rasteron_Image* sourceImage;
Rasteron_Image* greyImage;
Rasteron_Image* redImage;
Rasteron_Image* blueImage;
Rasteron_Sprite* sprite;
Rasteron_Heightmap* heightmap;

void genTargetImagePath() {
	strcpy(targetImagePath, imagePath);
	strcat(targetImagePath, "\\");
	strcat(targetImagePath, imageName);	
	fixPathDashes(&targetImagePath);
}

BITMAP bmap1;
BITMAP bmap2;

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
	// Genertation Step
	genTargetImagePath();

	sourceImage = createImgRef(targetImagePath);
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