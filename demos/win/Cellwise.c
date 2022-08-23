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

// Rasteron_Image* patternImg;
// Rasteron_Image* patternImg2;
Rasteron_Image* patternImgHorz;
Rasteron_Image* patternImgVert;
Rasteron_Image* mapImg;
Rasteron_Image* fieldImg;
Rasteron_Image* stepImg;

unsigned callback2(unsigned target, unsigned nebrs[2]) {
	// TODO: Implement callback

	return RESULT_COLOR;
}

unsigned callback8(unsigned target, unsigned nebrs[8]) {
	// TODO: Implement callback

	return RESULT_COLOR;
}

unsigned callback_map(double x, double y){
	// TODO: Implement callback

	return RESULT_COLOR;
}

void genImages() {
	addColorPoint(&colorPointTable, 0xFFFF0000, 0.25f, 0.75f);
	addColorPoint(&colorPointTable, 0xFF00FF00, 0.5f, 0.5f);
	addColorPoint(&colorPointTable, 0xFF0000FF, 0.75f, 0.25f);
	darkSwatch = createSwatch(BLACK_COLOR, 0x25);
	lightSwatch = createSwatch(WHITE_COLOR, 0x25);
	seedTable1 = createSeedTable(&lightSwatch);
	seedTable2 = createSeedTable(&darkSwatch);

	solidImg = createSolidImg((ImageSize){ 1100, 1200 }, BLACK_COLOR);
	seededImg = createSeedRawImg(solidImg, SEED_COLOR, 0.01);
	seededImg2 = createSeedWeightImg(solidImg, &seedTable2);

	// patternImg = createPatternImg_nebr(seededImg, callback8);
	// patternImg2 = createPatternImg_iter(seededImg, callback8, 4);
	patternImgHorz = createPatternImg_horz(seededImg, callback2);
	patternImgVert = createPatternImg_vert(seededImg, callback2);
	mapImg = createMappedImg((ImageSize){ 1100, 1200 }, callback_map);
	fieldImg = createFieldImg_vornoi((ImageSize){ 1100, 1200 }, &colorPointTable);
}

void cleanup() {
	deleteImg(solidImg);
	deleteImg(seededImg);
	deleteImg(seededImg2);

	// deleteImg(patternImg);
	// deleteImg(patternImg2);
	deleteImg(patternImgHorz);
	deleteImg(patternImgVert);
	deleteImg(mapImg);
	deleteImg(fieldImg);
}

BITMAP bmap;

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC = GetDC(hwnd);
	RECT rect;

	switch (message) {
	case (WM_CREATE): {
		bmap = createWinBmap(mapImg);
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