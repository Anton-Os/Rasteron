#include "Rasteron.h"

char fullImagePath[1024];

Rasteron_Image* sourceImage1; // BMP Image
Rasteron_Image* sourceImage2; // TIFF Image
Rasteron_Image* sourceImage3; // PNG Image

int main(int argc, char** argv) {
    // Reading Step

    genFullFilePath("User.bmp", &fullImagePath);
    sourceImage1 = createRefImg(fullImagePath);
    printf("%s image successfully loaded!", sourceImage1->name);
    genFullFilePath("User.tiff", &fullImagePath);
    sourceImage2 = createRefImg(fullImagePath);
    printf("%s image successfully loaded!", sourceImage2->name);
    genFullFilePath("User.png", &fullImagePath);
    sourceImage3 = createRefImg(fullImagePath);
    printf("%s image successfully loaded!", sourceImage3->name);
    
    // Cleanup Step
    
    deleteImg(sourceImage1);
    deleteImg(sourceImage2);
    deleteImg(sourceImage3);

    return 0;
}