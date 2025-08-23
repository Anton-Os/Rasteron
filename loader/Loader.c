#include <string.h>

#include "Loader.h"

enum IMG_FileFormat getFormat(const char* fileName) {
    size_t len = strlen(fileName);
    char extension[] = {
        *(fileName + len - 3),
        *(fileName + len - 2),
        *(fileName + len - 1),
		'\0'
    }; // gets the last 3 letters of a file extension

    if(!strcmp(extension, "bmp")) // bmp file check
        return IMG_Bmp;
    else if(! strcmp(extension, "png")) // png file check
        return IMG_Png;
    else if(! strcmp(extension, "iff") || ! strcmp(extension, "tif")) // tiff file check
        return IMG_Tiff;
    else return IMG_NonValid;
}

Rasteron_Image* loadImgOp(const char* fileName){
    enum IMG_FileFormat format = getFormat(fileName);
    Rasteron_Image* loadImg;

    switch(format){
        case IMG_Bmp: loadImg = loadImgOp_bmp(fileName); break;
        case IMG_Tiff: loadImg = loadImgOp_tiff(fileName); break;
        case IMG_Png: loadImg = loadImgOp_png(fileName); break;
    }

    return loadImg;
}

void writeFileImageRaw(const char* fileName, enum IMG_FileFormat format, unsigned height, unsigned width, unsigned* data){
    switch(format){
#ifdef USE_IMG_PNG
	case IMG_Png:
        // if(!strcmp(fileName, "png")) strcat(fileName, ".png");
        writeFileImageRaw_png(fileName, height, width, data);
        break;
#endif
#ifdef USE_IMG_TIFF
	case IMG_Tiff:
        // if(!strcmp(fileName, "iff") || !strcmp(fileName, "tif")) strcat(fileName, ".tiff");
        writeFileImageRaw_tiff(fileName, height, width, data);
        break;
#endif
#ifdef USE_IMG_BMP
	case IMG_Bmp:
        // if(!strcmp(fileName, "bmp")) strcat(fileName, ".bmp");
        writeFileImageRaw_bmp(fileName, height, width, data);
        break;
#endif
	default: perror("Image Format not supported!"); break;
    }
}