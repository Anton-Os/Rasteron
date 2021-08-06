// #include <inttypes.h>
#include "Loader.h"

void seedRandGen(); // seeds the random number generator
void makeRasterColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal); // makes image data into uniform color
void changeRasterColor(uint32_t* raster, unsigned int pCount, uint32_t newColor, uint32_t oldColor); // switches oldColor to newColor in image
void switchRasterRB(uint32_t* raster, unsigned int pCount); // switch red and blue bits
void switchRasterGB(uint32_t* raster, unsigned int pCount); // switch green and blue bits
void switchRasterRG(uint32_t* raster, unsigned int pCount); // swotch red and green bits
uint8_t getLoColorBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t getHiColorBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel);

uint32_t grayify32(uint32_t refColor); // produces a 32 bit grey value based on provided reference color
uint8_t grayify8(uint32_t refColor); // produces a 8 bit grey value based on provided reference color
// uint32_t fract32(uint32_t refColor, double frac) // produces an 32 bit color value based on fraction of reference color
uint8_t fract8(uint8_t refColor, double frac); // produces an 8 bit color value based on fraction of reference color
uint32_t interpolateColor(uint32_t color1, uint32_t color2, double iVal);

#ifdef _WIN32 
	#ifndef WIN_INCLUDE
		#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
			#include <Windows.h>
		#endif
		#define WIN_INCLUDE
	#endif

	void createWindow(LPCTSTR name); // Window creation
	void eventLoop(); // Event loop
	BITMAP createWinBmap(const Image* image); // Helper for all image types
	BITMAP createWinBmapRaw(uint32_t width, uint32_t height, uint32_t* data); // Just dimensions and data are provided
	void drawWinBmap(HWND hwnd, const BITMAP* bmap);
	// void drawWinBmap(HWND hwnd, const Image* image);
#endif // Port later to some Internal.h header