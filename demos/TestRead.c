#include "Rasteron.h"

char fullImagePath[1024];

Rasteron_Image* sourceImage1; // BMP Image
Rasteron_Image* sourceImage2; // TIFF Image
Rasteron_Image* sourceImage3; // PNG Image

int main(int argc, char** argv) {
    // Reading Step

    genFullFilePath("User.bmp", &fullImagePath);
    sourceImage1 = loadImgOp((argc < 1)? fullImagePath : argv[1]);
    printf("%s image read operation successful", sourceImage1->name);
    genFullFilePath("User.tiff", &fullImagePath);
    sourceImage2 = loadImgOp((argc < 2)? fullImagePath : argv[2]);
    printf("%s image read operation successful", sourceImage2->name);
    genFullFilePath("User.png", &fullImagePath);
    sourceImage3 = loadImgOp((argc < 3)? fullImagePath : argv[3]);
    printf("%s image read operation successful", sourceImage3->name);
    
    // Cleanup Step
    
    dealloc_image(sourceImage1);
    dealloc_image(sourceImage2);
    dealloc_image(sourceImage3);

    return 0;
}