#include "Util_OS.h"

// Path Generation

void genFullFilePath(const char* name, char* fullFilePath){
    strcpy(fullFilePath, ASSETS_DIR);
    strcat(fullFilePath, "/");
    strcat(fullFilePath, name);
}

// Unix Specific

void createWindow(Platform_Context* context, const char* name, unsigned width, unsigned height){
    context->display = XOpenDisplay(NULL);
    unsigned long whitePix = WhitePixel(context->display, DefaultScreen(context->display));
    unsigned long blackPix = BlackPixel(context->display, DefaultScreen(context->display));

    int screenNum = DefaultScreen(context->display);
    context->window = XCreateSimpleWindow(
        context->display, RootWindow(context->display, screenNum),
        50, 0,
        width, height,
        1, 0, 0
        // 1, blackPix, whitePix
    );
    context->visual = DefaultVisual(context->display, screenNum);
    context->gc = &DefaultGC(context->display, screenNum);
    context->depth = DefaultDepth(context->display, screenNum);

    XSelectInput(context->display, context->window, KeyPressMask | KeyReleaseMask | ExposureMask |ButtonPressMask);

    XMapWindow(context->display, context->window);
    XFlush(context->display);
    XFlushGC(context->display, *context->gc);
}

#include <stdio.h>

void eventLoop(Display* display, Window window, eventLoopCallback callback){
    static char lastKey = ' ';
    static double cursorPos[2] = { 0.0, 0.0 };
    static long time = 0;

    XEvent event;
    while(1){
        XNextEvent(display, &event);

        switch(event.type){
        case KeyRelease:
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            lastKey = keysym;
            break;
        // case ButtonPress: puts("Mouse button pressed"); break;
        case ButtonPress:
            Window root = RootWindow(display, DefaultScreen(display));
            int rootX, rootY, childX, childY;
            unsigned mask;
            XQueryPointer(display, root, &window, &root, &rootX, &rootY, &childX, &childY, &mask);
            cursorPos[0] = (double)childX;
            cursorPos[1] = (double)childY;
            break;
        default: break;
    }

    if(callback != NULL) callback(lastKey, cursorPos); // process any user interaction here
    }
}

XImage* createUnixBmapRaw(Platform_Context* context, uint32_t height, uint32_t width, uint32_t* data){
    XImage* image = XCreateImage(
        context->display, context->visual, context->depth,
        ZPixmap, 0, (char*)data, width, height, 32, 0
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
        context->display, context->window, *context->gc,
        image,
        0, 0, 0, 0,
        image->width, image->height
    );
}
