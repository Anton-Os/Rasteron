#include "Rasteron.h"

char fullImagePath[1024];

Rasteron_Image* sourceImage1; // BMP Image
Rasteron_Image* sourceImage2; // TIFF Image
Rasteron_Image* sourceImage3; // PNG Image

void genFullImagePath(const char* name) {
	strcpy(fullImagePath, IMAGE_DIR);
	strcat(fullImagePath, "/");
	strcat(fullImagePath, name);
#ifdef _WIN32
    fixPathDashes(&fullImagePath);
#endif
    printf("Image Path: %s \n", fullImagePath);
}

int main(int argc, char** argv) {
    // Reading Step

    genFullImagePath("Logo.bmp");
    sourceImage1 = createRefImg(fullImagePath);
    genFullImagePath("Logo.tiff");
    sourceImage2 = createRefImg(fullImagePath);
    genFullImagePath("Logo.png");
    sourceImage3 = createRefImg(fullImagePath);
    
    // Cleanup Step
    
    deleteImg(sourceImage1);
    deleteImg(sourceImage2);
    deleteImg(sourceImage3);

    return 0;
}