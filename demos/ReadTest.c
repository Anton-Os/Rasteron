#include "Rasteron.h"

char fullImagePath[1024];

Rasteron_Image* sourceImage1; // BMP Image
Rasteron_Image* sourceImage2; // TIFF Image
Rasteron_Image* sourceImage3; // PNG Image

int main(int argc, char** argv) {
    // Reading Step

    genFullFilePath("Logo.bmp", &fullImagePath);
    sourceImage1 = createRefImg(fullImagePath);
    genFullFilePath("Logo.tiff", &fullImagePath);
    sourceImage2 = createRefImg(fullImagePath);
    genFullFilePath("Logo.png", &fullImagePath);
    sourceImage3 = createRefImg(fullImagePath);
    
    // Cleanup Step
    
    deleteImg(sourceImage1);
    deleteImg(sourceImage2);
    deleteImg(sourceImage3);

    return 0;
}