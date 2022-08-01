#include "Rasteron.h"

Rasteron_Image* blankImage;

int main(int argc, char** argv) {
    blankImage = createImgSolid((ImageSize){1024, 1024}, 0xFFFFFF00);

    writeFileImageRaw("Generated.bmp", IMG_Bmp, blankImage->height, blankImage->width, blankImage->data);
	writeFileImageRaw("Generated.tiff", IMG_Tiff, blankImage->height, blankImage->width, blankImage->data);
	writeFileImageRaw("Generated.png", IMG_Png, blankImage->height, blankImage->width, blankImage->data);
    
    return 0;
}