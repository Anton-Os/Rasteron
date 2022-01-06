#include "Toolbox.h"

void createWindow(UnixContext* context, const char* name){
	context->display = XOpenDisplay(NULL);
	unsigned long whitePix = WhitePixel(context->display, DefaultScreen(context->display));
	unsigned long blackPix = BlackPixel(context->display, DefaultScreen(context->display));

	int visualCount;
	XVisualInfo visualInfoTemplate;
	visualInfoTemplate.screen = XGetVisualInfo(context->display, VisualScreenMask, &visualInfoTemplate, &visualCount);

	context->window = XCreateSimpleWindow(
		context->display, DefaultRootWindow(context->display),
		100, 100, 1200, 1100,
		1, blackPix, whitePix
	);

	context->gc = XCreateGC(context->display, context->window, 0, NULL);

	XMapWindow(context->display, context->window);
	XFlush(context->display);
	XFlushGC(context->display, context->gc);
}

void eventLoop(){
	XEvent event;
    while(1){
        // XNextEvent(context->display, &event);

        switch(event.type){
		// case (KeyPress): { printf("Key press! %x\t", event.xkey.keycode); }
		case (ButtonPress): {  }
		case (MotionNotify): {  }
		}
    }
}

XImage* createUnixBmapRaw(UnixContext* context, uint32_t height, uint32_t width, uint32_t* data){
	XImage* image;

	size_t dataSize = width * height * sizeof(char) * 4;
	char* newData = (char*)malloc(dataSize); // 4 attributes per color value
	for(unsigned p = 0; p < dataSize; p++)
		switch (p % 4) // Copies data from cooresponding input data
		{
			case 0: *(newData + p) = *(data + (p / 4)) & ALPHA_BITS_MASK; break;
			case 1: *(newData + p) = *(data + (p / 4)) & RED_BITS_MASK; break;
			case 2: *(newData + p) = *(data + (p / 4)) & GREEN_BITS_MASK; break;
			case 3: *(newData + p) = *(data + (p / 4)) & BLUE_BITS_MASK; break;
		}

	int bytesPerLine = 8 * ((width + 7) / 8);
	image = XCreateImage(
		context->display, context->visualInfo->visual, 
		IMAGE_DEPTH, ZPixmap, 0, 
		(char*)newData, bytesPerLine,
		height, 8, 0
	);

	free(newData);
	return image;
}

/* BITMAP createUnixBmap(const Image* image) {
	switch (image->fileFormat) {
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		switchRasterRB(image->data.tiff.raster, image->data.tiff.width * image->data.tiff.length); // Should maybe move to ImgTIFF
		return createUnixBmapRaw(image->data.tiff.width, image->data.tiff.length, image->data.tiff.raster);
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		return createUnixBmapRaw(abs(image->data.bmp.width), abs(image->data.bmp.height), image->data.bmp.data);
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png): 
		return createUnixBmapRaw(image->data.png.width, image->data.png.height, image->data.png.rgbaData);
#endif
	default:
		puts("Image Format not yet supported!!!");
		break;
	}
	BITMAP bmap = { 0 };
	return bmap;
} */

void drawUnixBmap(UnixContext* context, XImage* image){
    Pixmap pixmap = XCreatePixmap(context->display, context->window, image->width, image->height, IMAGE_DEPTH);
	
	XPutImage(
		context->display, context->window, context->gc, 
		image,
		0, 0, 0, 0, 
		image->width, image->height
	);
}