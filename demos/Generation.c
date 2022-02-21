#include "Rasteron.h"

// Global Definitions
const char* imagePath = IMAGE_DIR;
const char* imageName = "Logo.png";
char targetImagePath[1024];

// Image img = { 0 };
Rasteron_Image* imageBase;
Rasteron_Image* imageGrey;
Rasteron_Image* imageRed;
Rasteron_Image* imageBlue;
Rasteron_Palette* palette;
Rasteron_Palette* palette2;
Rasteron_Sprite* sprite;
// Rasteron_Heightmap* heightmap;

void genImageFilePath() {
	strcpy(targetImagePath, imagePath);
	strcat(targetImagePath, "\\");
	strcat(targetImagePath, imageName);	
	fixPathDashes(&targetImagePath);
}

void genImages(){
	genImageFilePath();

	//loadFileImage(targetImagePath, &img); // hard path
	imageBase = createImgRef(targetImagePath);
	imageGrey = createImgGrey(imageBase);
	imageRed = createImgFilter(imageBase, CHANNEL_Red);
	imageBlue = createImgFilter(imageBase, CHANNEL_Blue);
	palette = createPalette(imageBase);
	palette2 = filterPalette(DEFAULT_PALETTE_FILTER, palette);
	sprite = createSprite(imageBase);
	// heightmap = createHeightmap(imageGrey); // Lattice data test
}

void cleanup() {
	deleteSprite(sprite);
	deleteImg(imageBase);
	deleteImg(imageGrey);
	deleteImg(imageRed);
	deleteImg(imageBlue);
	// deleteHeightmap(heightmap);
	deletePalette(palette);
	deletePalette(palette2);
	// delFileImage(&img);
}

#ifdef _WIN32

BITMAP bmap1;
BITMAP bmap2;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		genImages();

		// bmap1 = createWinBmap(&img);
		bmap2 = createWinBmap(imageRed);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap2);
	}
	case (WM_CLOSE): {}
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

#elif defined __linux__

void unixProc(){
	UnixContext context;
	createWindow(&context, "Primitives");
	genImages();

	XImage* unixBmap = createUnixBmapRaw(context, imageRed->width, imageRed->height, imageRed->data);
	drawUnixBmap(context, unixBmap);
}

#endif

int main(int argc, char** argv) {

#ifdef _WIN32
	createWindow(wndProc, "Primitives");
#elif defined __linux__
	unixProc();
#endif
	eventLoop();

	cleanup(); // cleanup step
	return 0;
}