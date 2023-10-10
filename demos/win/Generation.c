#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
const char* imageName = "User.png";
char fullImagePath[1024];

Rasteron_Image* sourceImage;
Rasteron_Image* greyImage;
Rasteron_Image* redImage;
Rasteron_Image* blueImage;
Rasteron_Sprite* sprite;
Rasteron_Heightmap* heightmap;

void genImages(){
	sourceImage = createRefImg(fullImagePath);
	greyImage = createGreyscaleImg(sourceImage);
	redImage = createFilterImg(sourceImage, CHANNEL_Red);
	blueImage = createChanneledImg(sourceImage, CHANNEL_Blue);
	sprite = createSprite(sourceImage);
	heightmap = createRefHeightmap(sourceImage);
}

void cleanup(){
	free_image(sourceImage);
	free_image(greyImage);
	free_image(redImage);
	free_image(blueImage);
	free_sprite(sprite);
	free_heightmap(heightmap);
}

BITMAP bmap1, bmap2, bmap3;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): { bmap1 = createWinBmap(sourceImage); }
	case (WM_PAINT): { drawWinBmap(hwnd, &bmap1); }
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}


int main(int argc, char** argv) {
	// Genertation Step
	
	genFullFilePath(imageName, &fullImagePath);
	genImages();

	// Event Loop

	createWindow(wndProc, "Generation");
	eventLoop();

	// Cleanup Step

	cleanup();

	return 0;
}