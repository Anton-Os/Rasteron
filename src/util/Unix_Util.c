#include "OS_Util.h"

// Path Generation

void genFullFilePath(const char* name, char* fullFilePath){
	strcpy(fullFilePath, ASSETS_DIR);
	strcat(fullFilePath, "/");
	strcat(fullFilePath, name);
}

// Unix Specific

void createWindow(Platform_Context* context, const char* name){
	context->display = XOpenDisplay(NULL);
	unsigned long whitePix = WhitePixel(context->display, DefaultScreen(context->display));
	unsigned long blackPix = BlackPixel(context->display, DefaultScreen(context->display));

	int screenNum = DefaultScreen(context->display);
	context->window = XCreateSimpleWindow(
		context->display, RootWindow(context->display, screenNum),
		50, 0, 1200, 1100,
		1, 0, 0
		// 1, blackPix, whitePix
	);
	context->visual = DefaultVisual(context->display, screenNum);
	context->gc = DefaultGC(context->display, screenNum);
	context->depth = DefaultDepth(context->display, screenNum);

	XSelectInput(context->display, context->window, ExposureMask);
	XMapWindow(context->display, context->window);
	XFlush(context->display);
	XFlushGC(context->display, context->gc);
}

void eventLoop(Display* display){
	XEvent event;
    while(1){
        XNextEvent(display, &event);

        switch(event.type){
		case (KeyPress): return;
		}
    }
}

XImage* createUnixBmapRaw(Platform_Context* context, uint32_t height, uint32_t width, uint32_t* data){
	XImage* image = XCreateImage(
		context->display, context->visual, context->depth,
		ZPixmap, 0, data, width, height, 32, 0
	);
	
	for(unsigned h = 0; h < height; h++)
		for(unsigned w = 0; w < width; w++)
			XPutPixel(image, w, h, *(data + (h * width) + w));

	return image;
}

XImage* createUnixBmap(Platform_Context* context, Rasteron_Image* image){
	createUnixBmapRaw(context, image->height, image->width, image->data);
}

void drawUnixBmap(Platform_Context* context, XImage* image){
	XPutImage(
		context->display, context->window, context->gc, 
		image,
		0, 0, 0, 0,
		image->width, image->height
	);
}