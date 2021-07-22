// #include <inttypes.h>
#include "Loader.h"

void makeRasterColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal);
void changeRasterColor(uint32_t* raster, unsigned int pCount, uint32_t newColor, uint32_t oldColor);
void switchRasterRB(uint32_t* raster, unsigned int pCount); // switch red and blue bits
void switchRasterGB(uint32_t* raster, unsigned int pCount); // switch green and blue bits
void switchRasterRG(uint32_t* raster, unsigned int pCount); // swotch red and green bits
uint32_t getRasterBkColor(uint32_t* raster, unsigned width, unsigned height);

uint32_t grayify32(uint32_t refColor);
uint8_t grayify8(uint32_t refColor);
uint32_t interpolateColor(uint32_t color1, uint32_t color2, double iVal);

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