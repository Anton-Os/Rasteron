#include "Rasteron.h"
#include "OS_Util.h"

Rasteron_Image* blankImage;

int main(int argc, char** argv) {
    // Genertation Step

    blankImage = createImgBlank(1100, 1200, 0xFFFF0000);

    // Event Loop

    Platform_Context platformContext;
    createWindow(&platformContext, "OpenSesame");

    // XImage* bmap = createUnixBmapRaw(&platformContext, blankImage->height, blankImage->width, blankImage->data);
    XImage* bmap = createUnixBmap(&platformContext, blankImage);
    XEvent event;
	while(1) {
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}
    
    // Cleanup Step

    deleteImg(blankImage);
    return 0;
}