#include "Rasteron.h"

const char* imagePath = IMAGE_DIR;
Rasteron_Image* sourceImage1; // BMP Image
Rasteron_Image* sourceImage2; // TIFF Image
Rasteron_Image* sourceImage3; // PNG Image

int main(int argc, char** argv) {

    // TODO: Include proper paths
    sourceImage1 = createImgRef("Logo.bmp");
    sourceImage2 = createImgRef("Logo.tiff");
    sourceImage3 = createImgRef("Logo.png");
    
    return 0;
}