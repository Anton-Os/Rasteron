#include "Rasteron.h"

Rasteron_Image* canvasImage;

int main(int argc, char** argv) {
    char inputBuff[6000];
    char escapeSeq = 'X';

    if(argc > 1) canvasImage = loadImgOp(argv[1]); // use first argument as reference image path
    else canvasImage = solidImgOp((ImageSize){1024, 1024 }, BLACK_COLOR); // black canvas by default

    puts("\tPainter running...\n");
    while(inputBuff[0] != '\n'){ // TODO: parse commands and draw on canvas image
        scanf("%s", inputBuff);
        printf("\tInput text: %s\n", inputBuff);
    }

#ifdef USE_IMG_BMP
    writeFileImageRaw("Generated.bmp", IMG_Bmp, canvasImage->height, canvasImage->width, canvasImage->data);
#endif
#ifdef USE_IMG_TIFF
	writeFileImageRaw("Generated.tiff", IMG_Tiff, canvasImage->height, canvasImage->width, canvasImage->data);
#endif
#ifdef USE_IMG_PNG
	writeFileImageRaw("Generated.png", IMG_Png, canvasImage->height, canvasImage->width, canvasImage->data);
#endif

    free_image(canvasImage);
}