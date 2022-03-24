#define RASTERON_ENABLE_PLUGIN

#include "Rasteron.h"
#include "OS_Util.h"

// Global Definitions
const char* imagePath = IMAGE_DIR;
const char* imageName = "Logo.png";
char targetImagePath[1024];

Rasteron_Image* blankImg;
Rasteron_Image* seededImg;
Rasteron_Image* patternImgHorz;
Rasteron_Image* patternImgVert;
Rasteron_Image* sourceImage;

#define SEED_COLOR 0xFFFFFFFF

unsigned horzCallback(unsigned right, unsigned left){
    return ZERO_COLOR;
}

unsigned vertCallback(unsigned right, unsigned left){
    return ZERO_COLOR;
}

void genTargetImagePath() {
	strcpy(targetImagePath, imagePath);
	strcat(targetImagePath, "/");
	strcat(targetImagePath, imageName);
}

int main(int argc, char** argv) {
    // Genertation Step
    genTargetImagePath();

    blankImg = createImgBlank(1100, 1200, BLACK_COLOR);
	sourceImage = createImgRef(targetImagePath);
    seededImg = createImgSeedRaw(blankImg, SEED_COLOR, 0.1);

    patternImgHorz = createHorzPatImg(seededImg, horzCallback);
    patternImgVert = createVertPatImg(seededImg, vertCallback);

    // Event Loop

    Platform_Context platformContext;
    createWindow(&platformContext, "Experiment");

    XImage* bmap = createUnixBmap(&platformContext, blankImg);
    XEvent event;
	while(1) {
		XNextEvent(platformContext.display, &event);
		if(event.type == Expose)
		    drawUnixBmap(&platformContext, bmap);
	}

    // Cleanup Step

    deleteImg(blankImg);
    deleteImg(seededImg);
    deleteImg(patternImgHorz);
    deleteImg(patternImgVert);
    return 0;
}