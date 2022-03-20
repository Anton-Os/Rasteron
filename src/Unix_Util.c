#include "OS_Util.h"

void createWindow(Platform_Context* context, const char* name){
	context->display = XOpenDisplay(NULL);
	unsigned long whitePix = WhitePixel(context->display, DefaultScreen(context->display));
	unsigned long blackPix = BlackPixel(context->display, DefaultScreen(context->display));

	int screenNum = DefaultScreen(context->display);
	context->window = XCreateSimpleWindow(
		context->display, RootWindow(context->display, screenNum),
		50, 0, 1200, 1100,
		1, blackPix, whitePix
	);
	context->visual = DefaultVisual(context->display, screenNum);

	// context->gc = XCreateGC(context->display, context->window, 0, NULL);
	context->gc = DefaultGC(context->display, screenNum);

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
	// size_t dataSize = width * height * 4;
	// unsigned char* imageData = (unsigned char*)malloc(dataSize); // 4 attributes per color value
	/* for(unsigned p = 0; p < dataSize; p++)
		switch (p % 4){ // copies data from cooresponding input data
			case 0: *(imageData + p) = *(data + (p / 4)) & ALPHA_CHANNEL; break;
			case 1: *(imageData + p) = *(data + (p / 4)) & RED_CHANNEL; break;
			case 2: *(imageData + p) = *(data + (p / 4)) & GREEN_CHANNEL; break;
			case 3: *(imageData + p) = *(data + (p / 4)) & BLUE_CHANNEL; break;
		} */

	XImage* image = XCreateImage(
		context->display, context->visual,
		IMAGE_DEPTH, ZPixmap, 0, data,
		// width, height, 32, 0
		256, 256, 32, 0
	);
	
	/* for(unsigned h = 0; h < height; h++)
		for(unsigned w = 0; w < width; w++)
			XPutPixel(image, w, h, BLACK_COLOR); */
	for(unsigned p = 0; p < width * height; p++)
		XAddPixel(image, (long)p);

	// free(imageData);
	return image;
}

void drawUnixBmap(Platform_Context* context, XImage* image){
    XPutImage(
		context->display, context->window, context->gc, 
		image,
		0, 0, 0, 0, 
		// image->width, image->height
		256, 256
	);
}