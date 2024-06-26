#include "Rasteron.h"

#define SIZE 1024

static Rasteron_Image* sampleImgOp(){
    Rasteron_Image* sampleImg = solidImgOp((ImageSize){SIZE, SIZE}, 0xFFFFFF00); // yellow
    
    for(unsigned p = 0; p < SIZE * SIZE; p++) 
        if(p > (SIZE * SIZE / 2)) *(sampleImg->data + p) = (p % SIZE > SIZE / 2)? 0xFFFF00FF : 0xFF00FFFF;

    return sampleImg;
}

static void writeImageTest(const char* filePath, enum IMG_FileFormat format){
    char outputFileName[1024];
    strcpy(outputFileName, filePath);
#ifdef _WIN32
    replaceFwdSlash(outputFileName);
#endif

    Rasteron_Image* sampleImg = loadImgOp(outputFileName);
    switch(format){
        case IMG_Bmp: strcat(outputFileName, ".bmp"); break;
        case IMG_Tiff: strcat(outputFileName, ".tiff"); break;
        case IMG_Png: strcat(outputFileName, ".png"); break;
    }

    writeFileImageRaw(outputFileName, format, sampleImg->height, sampleImg->width, sampleImg->data);
    RASTERON_DEALLOC(sampleImg);
}

int main(int argc, char** argv) {
    Rasteron_Image* sampleImg = (argc <= 1)? sampleImgOp() : NULL;

    // Writing Step

    (argc <= 1)? puts("Writing default image to Output.bmp, Output.tiff, and Output.png") 
               : printf("Writing multiple images for %s", argv[1]);
#ifdef USE_IMG_BMP
    if (argc <= 1) writeFileImageRaw("Output.bmp", IMG_Bmp, SIZE, SIZE, sampleImg->data);
    else for(unsigned i = 0; i < argc - 1; i++) writeImageTest(argv[i + 1], IMG_Bmp);
#endif
#ifdef USE_IMG_TIFF
    if (argc <= 1) writeFileImageRaw("Output.tiff", IMG_Tiff, SIZE, SIZE, sampleImg->data);
    else for(unsigned i = 0; i < argc - 1; i++) writeImageTest(argv[i + 1], IMG_Tiff);
#endif
#ifdef USE_IMG_PNG
    if(argc <= 1) writeFileImageRaw("Output.png", IMG_Png, SIZE, SIZE, sampleImg->data);
    else for(unsigned i = 0; i < argc - 1; i++) writeImageTest(argv[i + 1], IMG_Png);
#endif

    // Cleanup Step

    RASTERON_DEALLOC(sampleImg);

    return 0;
}