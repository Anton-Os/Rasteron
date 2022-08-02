#include "Rasteron.h"

#define IMAGE_SIZE 1024

Rasteron_Image* blankImage;
Rasteron_Image* splitImage;

int main(int argc, char** argv) {
    // Generation Step

    blankImage = createImgSolid((ImageSize){IMAGE_SIZE, IMAGE_SIZE}, 0xFF666666);
    splitImage = createImgSolid((ImageSize){IMAGE_SIZE, IMAGE_SIZE}, 0xFFFF0000);
    for(unsigned p = 0; p < (IMAGE_SIZE * IMAGE_SIZE) / 2; p++) *(splitImage->data + p) = 0xFF0000FF;

    // Writing Step

    writeFileImageRaw("Generated.bmp", IMG_Bmp, IMAGE_SIZE, IMAGE_SIZE, splitImage->data);
	writeFileImageRaw("Generated.tiff", IMG_Tiff, IMAGE_SIZE, IMAGE_SIZE, splitImage->data);
	writeFileImageRaw("Generated.png", IMG_Png, IMAGE_SIZE, IMAGE_SIZE, splitImage->data);

    // Cleanup Step

    deleteImg(blankImage);
    deleteImg(splitImage);

    return 0;
}