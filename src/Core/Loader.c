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

void rstnLoad(const char* fileName, Image* image){
    enum IMG_FileFormat format = getFormat(fileName);
    
    switch(format){
        case IMG_Png:
            loadImage_PNG(fileName, image);
            break;
        case IMG_Tiff:
            loadImage_TIFF(fileName, image);
            break;
        case IMG_Bmp:
            loadImage_BMP(fileName, image);
            break;
        default:
            break; // Probably need some kind of error
    }
}

void rstnDel(Image* image) {
	switch (image->fileFormat) {
	case IMG_Png:
		delImage_PNG(image);
		break;
	case IMG_Tiff:
		delImage_TIFF(image);
		break;
	case IMG_Bmp:
		delImage_BMP(image);
		break;
	default:
		break;
	}
}