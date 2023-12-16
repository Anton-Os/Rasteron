#include "Rasteron.h"

void genFullFilePath(const char* name, char* fullFilePath);

typedef void (*eventLoopCallback)(void);

#ifdef _WIN32 
	#define TIMER_EVENT_ID 1
 
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	void createWindow(WNDPROC wndProc, LPCTSTR name, unsigned width, unsigned height); // Window creation
	void eventLoop(eventLoopCallback callback); // Event loop
	BITMAP createWinBmapRaw(uint32_t height, uint32_t width, uint32_t* data); // raw data call
	BITMAP createWinBmap(Rasteron_Image* image); // formatted data call
    void drawWinBmap(HWND hwnd, const BITMAP* bmap);
	// void drawWinBmap(HWND hwnd, const Image* image);
#elif defined __linux__
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <unistd.h>
	#include <malloc.h>
	#include <stdio.h>

	typedef struct {
		Display* display;
		GC* gc;
		// XVisualInfo* visualInfo;
		Visual* visual;
		Window window;
		// XGCValues gcVals;
		int depth;
	} Platform_Context;

	void createWindow(Platform_Context* context, const char* name, unsigned width, unsigned height); // Window creatiom
	void eventLoop(Display* display, eventLoopCallback callback); // Event loop
	// XImage createUnixBmap(const Image* image); // formatted data call
	XImage* createUnixBmapRaw(Platform_Context* context, uint32_t height, uint32_t width, uint32_t* data); // raw data call
	XImage* createUnixBmap(Platform_Context* context, Rasteron_Image* image);
	void drawUnixBmap(Platform_Context* context, XImage* image);
#endif // Port later to some Internal.h header