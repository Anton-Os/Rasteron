#define RASTERON_ENABLE_PLUGIN
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions

const char* fontName = "New-Tegomin.ttf";
char fullFontPath[1024];
Rasteron_Text textObj;

Rasteron_Image* fontImage;
Rasteron_Image* flipImage;

void genImages(){
    textObj.bkColor = genRandColorVal();
	textObj.fgColor = genRandColorVal();
	textObj.fileName = &fullFontPath;
	textObj.text = "Rasteron is Dope!";

    fontImage = bakeText(&textObj);
	flipImage = createFlipImg(fontImage, FLIP_Clock);
}

void cleanup(){
    free_image(fontImage);
    free_image(flipImage);
}

int main(int argc, char** argv) {
    // Genertation Step

    srand(time(NULL));
    genFullFilePath(fontName, &fullFontPath);
    genImages();

    // Event Loop

    Platform_Context platformContext;
    createWindow(&platformContext, "Typo");

    XImage* bmap = createUnixBmap(&platformContext, fontImage);
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