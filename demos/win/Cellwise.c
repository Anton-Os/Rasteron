#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

#define SEED_COLOR 0xFFFFFFFF
#define RESULT_COLOR 0xFF00FF00

Rasteron_Swatch darkSwatch, lightSwatch;
Rasteron_SeedTable seedTable1, seedTable2;
ColorPointTable colorPointTable;

Rasteron_Image* solidImg;
Rasteron_Image* seededImg;
Rasteron_Image* seededImg2;
Rasteron_Image* seededImg3;

Rasteron_Image* mapImg;
Rasteron_Image* patternImg;
Rasteron_Image* iterativeImg;
Rasteron_Image* horzImg;
Rasteron_Image* vertImg;
Rasteron_Image* fieldImg;
Rasteron_Image* stepImg;
Rasteron_Image* mixImg;

unsigned callback2(unsigned target, unsigned nebrs[2]) {
	return ((double)rand() / (double)RAND_MAX < 0.5)? nebrs[0] : nebrs[1]; // 50% chance to copy either neighbor
}

unsigned callback8(unsigned target, unsigned nebrs[8]) {
	if(target == SEED_COLOR) return RESULT_COLOR;

	for(unsigned n = 0; n < NEBR_COUNT; n++)
		if(nebrs[n] == SEED_COLOR) return SEED_COLOR;

	return ZERO_COLOR;
}

unsigned callback_field(unsigned color, double distance){ return blend(BLACK_COLOR, color, distance); }

unsigned callback_map(double x, double y){ return blend(BLACK_COLOR, WHITE_COLOR, x); } // simple gradient

ColorStep callback_step(const NebrTable* nebrTable, ColorStep prevStep, unsigned short stepCount){ return (ColorStep){ SEED_COLOR, NBR_None }; }

void genImages() {
	addColorPoint(&colorPointTable, 0xFFFF0000, 0.25f, 0.75f);
	addColorPoint(&colorPointTable, 0xFF00FF00, 0.5f, 0.5f);
	addColorPoint(&colorPointTable, 0xFF0000FF, 0.75f, 0.25f);

	lightSwatch = createSwatch(WHITE_COLOR, 0x25);
	darkSwatch = createSwatch(BLACK_COLOR, 0x25);
	seedTable1 = createSeedTable(&lightSwatch);
	seedTable2 = createSeedTable(&darkSwatch);

	solidImg = createSolidImg((ImageSize){ 1100, 1200 }, 0xFF0000FF);
	seededImg = createSeedRawImg(solidImg, SEED_COLOR, 0.01);
	seededImg2 = createSeedWeightImg(solidImg, &seedTable1);
	seededImg3 = createSeedWeightImg(solidImg, &seedTable2);

	mapImg = createMappedImg((ImageSize){ 1100, 1200}, callback_map);
	patternImg = createPatternImg(seededImg, callback8);
	iterativeImg = createMultiPatternImg(seededImg, callback8, 5);
	horzImg = createPatternImg_horz(seededImg2, callback2);
	vertImg = createPatternImg_vert(seededImg3, callback2);
	fieldImg = createFieldImg((ImageSize){ 1100, 1200 }, &colorPointTable, callback_field);
	// fieldImg = createFieldImg_vornoi((ImageSize){ 1100, 1200 }, &colorPointTable);
	mixImg = createFuseImg(horzImg, seededImg2);
}

void cleanup() {
	deleteImg(solidImg);
	deleteImg(seededImg);
	deleteImg(seededImg2);
	deleteImg(seededImg3);

	deleteImg(mapImg);
	deleteImg(patternImg);
	deleteImg(iterativeImg);
	deleteImg(horzImg);
	deleteImg(vertImg);
	deleteImg(fieldImg);
	deleteImg(mixImg);
}

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap = createWinBmap(mixImg);
	}
	case (WM_PAINT): {
		drawWinBmap(hwnd, &bmap);
	}
	case (WM_DESTROY): { }
	default: return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

int main(int argc, char** argv) {
	// Genertation Step

	genImages();

	// Event Loop

	createWindow(wndProc, "Cellwise");
	eventLoop((ImageSize){ 1100, 1200 });

	// Cleanup Step

	cleanup();

	return 0;
}