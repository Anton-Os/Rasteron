#include "Image.h"

#ifdef _WIN32 
	#define TIMER_EVENT_ID 1
 
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>

	void createWindow(WNDPROC wndProc, LPCTSTR name); // Window creation
	void eventLoop(); // Event loop
	BITMAP createWinBmapRaw(uint32_t height, uint32_t width, uint32_t* data); // raw data call
	BITMAP createWinBmap(Rasteron_Image* image); // formatted data call
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