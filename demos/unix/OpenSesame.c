#include "Rasteron.h"
#include "OS_Util.h"

Rasteron_Image* imageBlank;

int main(int argc, char** argv) {
    // Genertation Step

    imageBlank = createImgBlank(256, 256, 0xFFFF0000);

    // Event Loop

    Platform_Context platformContext;
    createWindow(&platformContext, "Open");

    XImage* bmap = createUnixBmapRaw(&platformContext, imageBlank->height, imageBlank->width, imageBlank->data);
    XEvent event;
	while(1)
	{
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}
    

    sleep(10); // eventLoop(platformContext.display);

    // Cleanup Step

    deleteImg(imageBlank);
    return 0;
}