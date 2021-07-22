// #include <inttypes.h>
#include "Loader.h"

void switchBits_RB(uint32_t* raster, unsigned int pCount);// Utility Function

void makeSolidColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal);
void changeToSolidColor(uint32_t* raster, unsigned int pCount, uint32_t newClr, uint32_t oldClr);

uint32_t grayify32(uint32_t refClr);
uint32_t getBkColor(uint32_t* raster, unsigned width, unsigned height);

// void createImage(const char* fileName, Image* image);

#ifdef _WIN32 
	#ifndef WIN_INCLUDE
		#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
			#include <Windows.h>
		#endif
		#define WIN_INCLUDE
	#endif

	BITMAP createWinBmap(const Image* image); // Helper for all image types
	BITMAP createWinBmapRaw(uint32_t width, uint32_t height, uint32_t* data); // Just dimensions and data are provided
	void drawWinBmap(HWND hwnd, const BITMAP* bmap);
	// void drawWinBmap(HWND hwnd, const Image* image);
#endif // Port later to some Internal.h header