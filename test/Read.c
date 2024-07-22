#include "Rasteron.h"

char fullImagePath[1024];

Rasteron_Image* sourceImage1; // BMP Image
Rasteron_Image* sourceImage2; // TIFF Image
Rasteron_Image* sourceImage3; // PNG Image

int main(int argc, char** argv) {
    // Reading Step

    genFullFilePath("Zero+.bmp", &fullImagePath);
    sourceImage1 = loadImgOp((argc < 2)? fullImagePath : argv[1]);
    printf("%s image read operation successful, height is %d, width is %d", sourceImage1->name, sourceImage1->height, sourceImage1->width);
    genFullFilePath("User.tiff", &fullImagePath);
    sourceImage2 = loadImgOp((argc < 3)? fullImagePath : argv[2]);
    printf("%s image read operation successful", sourceImage2->name);
    genFullFilePath("User.png", &fullImagePath);
    sourceImage3 = loadImgOp((argc < 4)? fullImagePath : argv[3]);
    printf("%s image read operation successful", sourceImage3->name);
    
    // Cleanup Step
    
    RASTERON_DEALLOC(sourceImage1);
    RASTERON_DEALLOC(sourceImage2);
    RASTERON_DEALLOC(sourceImage3);

    return 0;
}