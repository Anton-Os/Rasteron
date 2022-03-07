#include <math.h>
#include <string.h>

#include "Loader.h"

void seedRandGen(); // seeds the random number generator
void fixPathDashes(char* str); // utility for replacing forward-slashes with back-slashes

void makeColor(uint32_t* raster, unsigned pixels, uint32_t colorVal); // makes image data into uniform color
void changeColor(uint32_t* raster, unsigned pixels, uint32_t newColor, uint32_t oldColor); // switches oldColor to newColor in image
void switchRB(uint32_t* raster, unsigned pixels); // switch red and blue bits
void switchGB(uint32_t* raster, unsigned pixels); // switch green and blue bits
void switchRG(uint32_t* raster, unsigned pixels); // switch red and green bits
uint32_t genRandColorVal();
uint8_t getLoChannelBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t getHiChannelBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
int8_t getLightDiff(uint32_t color1, uint32_t color2); // returns positive if lighter, negative if darker, zero on match
int8_t getChannelDiff(uint32_t color1, uint32_t color2, CHANNEL_Type channel); // returns positive if lighter, negative if darker, zero on match
double getPixDist(unsigned p1, unsigned p2, unsigned imageWidth); // gets pixel distance based on indices

uint8_t grayify8(uint32_t refColor); // produces a 8 bit grey value based on reference color
uint32_t grayify32(uint32_t refColor); // produces a 32 bit grey value based on reference color
uint8_t fract8(uint8_t refColor, double frac); // produces an 8 bit color value based on fraction of reference color
uint32_t fract32(uint32_t refColor, double frac); // produces an 32 bit color value based on fraction of reference color
uint32_t blend(uint32_t color1, uint32_t color2, double bVal); // blends values between two colors
uint32_t itrpolate(uint32_t color1, uint32_t color2, double iVal); // interpolate between computed hi and low colors

#ifdef _WIN32 
	#define TIMER_EVENT_ID 1
 
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	void createWindow(WNDPROC wndProc, LPCTSTR name); // Window creation
	void eventLoop(); // Event loop
	BITMAP createWinBmap(const Image* image); // formatted data call
	BITMAP createWinBmapRaw(uint32_t height, uint32_t width, uint32_t* data); // raw data call
	void drawWinBmap(HWND hwnd, const BITMAP* bmap);
	// void drawWinBmap(HWND hwnd, const Image* image);
#elif defined __linux__
	#include<X11/X.h>
	#include<X11/Xlib.h>
	#include<X11/Xutil.h>
	#include<X11/Xresource.h>

	#define IMAGE_DEPTH 1

	typedef struct {
		Display* display;
		XVisualInfo* visualInfo;
		Window window;
		// XGCValues gcVals;
		GC* gc;
	} UnixContext;

	void createWindow(UnixContext* context, const char* name); // Window creatiom
	void eventLoop(); // Event loop
	// XImage createUnixBmap(const Image* image); // formatted data call
	XImage* createUnixBmapRaw(UnixContext* context, uint32_t height, uint32_t width, uint32_t* data); // raw data call
	void drawUnixBmap(UnixContext* context, XImage* image);
#endif // Port later to some Internal.h header