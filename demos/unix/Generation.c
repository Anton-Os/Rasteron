#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
const char* imageName = "Logo.png";
char fullImagePath[1024];

Rasteron_Image* blankImage;
Rasteron_Image* sourceImage;

void genImages(){
    blankImage = createSolidImg((ImageSize){1100, 1200}, 0xFFFFFF00);
    sourceImage = createRefImg(fullImagePath);
}

void cleanup(){
    free_image(blankImage);
    free_image(sourceImage);
}

int main(int argc, char** argv) {
    // Genertation Step

    genFullFilePath(imageName, &fullImagePath);
    genImages();

    // Event Loop

    Platform_Context platformContext;
    createWindow(&platformContext, "Generation");

    XImage* bmap = createUnixBmap(&platformContext, sourceImage);
    XEvent event;
	while(1) {
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}
    
    // Cleanup Step

    cleanup();
    return 0;
}