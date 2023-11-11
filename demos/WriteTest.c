#include "Rasteron.h"

#define SIZE 1024

Rasteron_Image* blankImage;
Rasteron_Image* testImage;

Rasteron_Image* testImgOp(){
    Rasteron_Image* testImage = solidImgOp((ImageSize){SIZE, SIZE}, 0xFFFF0000); // red
    
    for(unsigned p = 0; p < (SIZE * SIZE) / 2; p++)
        *(testImage->data + p) = 0xFF0000FF; // blue

    for(unsigned p = 0; p < (SIZE * SIZE) / 2; p++)
        if(p % SIZE > (SIZE / 2) - (SIZE / 4))
            *(testImage->data + p) = 0xFF00FF00; // green

    return testImage;
}

int main(int argc, char** argv) {
    // Generation Step

    blankImage = solidImgOp((ImageSize){SIZE, SIZE}, 0xFF666666);
    testImage = testImgOp();

    // Writing Step

#ifdef USE_IMG_BMP
    writeFileImageRaw("Generated.bmp", IMG_Bmp, SIZE, SIZE, testImage->data);
#endif
#ifdef USE_IMG_TIFF
	writeFileImageRaw("Generated.tiff", IMG_Tiff, SIZE, SIZE, testImage->data);
#endif
#ifdef USE_IMG_PNG
	writeFileImageRaw("Generated.png", IMG_Png, SIZE, SIZE, testImage->data);
#endif  

    // Cleanup Step

    dealloc_image(blankImage);
    dealloc_image(testImage);

    return 0;
}