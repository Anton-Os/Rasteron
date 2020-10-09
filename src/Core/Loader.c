#include "string.h"

#include "Rasteron.h"

static enum IMG_FileFormat getFormat(const char* fileName) {
    size_t len = strlen(fileName);
    char extensionTarget[] = {
        *(fileName + len - 3),
        *(fileName + len - 2),
        *(fileName + len - 1),
		'\0'
    }; // Gets the last 3 letters of a file extension

    char extension_png[] = "png";
    char extension_tiff[] = "tif";
    char extension_bmp[] = "bmp";

    if(! strcmp(extensionTarget, extension_png))
        return IMG_Png;
    else if(! strcmp(extensionTarget, extension_tiff))
        return IMG_Tiff;
    else if(! strcmp(extensionTarget, extension_bmp))
        return IMG_Bmp;
    else
        return IMG_NonValid;
}

void rstnLoadFromFile(const char* fileName, FileImage* image){
    enum IMG_FileFormat format = getFormat(fileName);
    
    switch(format){

	case IMG_Png:
#ifdef USE_IMG_PNG
		loadImage_PNG(fileName, image);
#endif // TODO: Error check
		break;

	case IMG_Tiff:
#ifdef USE_IMG_TIFF
		loadImage_TIFF(fileName, image);
#endif // TODO: Error check
		break;

	case IMG_Bmp:
#ifdef USE_IMG_BMP
		loadImage_BMP(fileName, image);
#endif // TODO: Error check
		break;

	default:
		break;
    }
}

void rstnDelFromFile(FileImage* image) {
	switch (image->fileFormat) {

	case IMG_Png:
#ifdef USE_IMG_PNG
		delImage_PNG(image);
#endif // TODO: Error check
		break;

	case IMG_Tiff:
#ifdef USE_IMG_TIFF
		delImage_TIFF(image);
#endif // TODO: Error check
		break;

	case IMG_Bmp:
#ifdef USE_IMG_BMP
		delImage_BMP(image);
#endif // TODO: Error check
		break;

	default:
		break;
	}
}