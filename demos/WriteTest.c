#include "Rasteron.h"

#define SIZE 1024

Rasteron_Image* blankImage;
Rasteron_Image* colorImage;

Rasteron_Image* genTestImage(){
    Rasteron_Image* testImage = createSolidImg((ImageSize){SIZE, SIZE}, 0xFFFF0000); // red
    
    for(unsigned p = 0; p < (SIZE * SIZE) / 2; p++)
        *(testImage->data + p) = 0xFF0000FF; // blue

    for(unsigned p = 0; p < (SIZE * SIZE) / 2; p++)
        if(p % SIZE > (SIZE / 2) - (SIZE / 4))
            *(testImage->data + p) = 0xFF00FF00; // green

    return testImage;
}

int main(int argc, char** argv) {
    // Generation Step

    blankImage = createSolidImg((ImageSize){SIZE, SIZE}, 0xFF666666);
    colorImage = genTestImage();

    // Writing Step

    writeFileImageRaw("Generated.bmp", IMG_Bmp, SIZE, SIZE, colorImage->data);
	writeFileImageRaw("Generated.tiff", IMG_Tiff, SIZE, SIZE, colorImage->data);
	writeFileImageRaw("Generated.png", IMG_Png, SIZE, SIZE, colorImage->data);

    // Cleanup Step

    deleteImg(blankImage);
    deleteImg(colorImage);

    return 0;
}