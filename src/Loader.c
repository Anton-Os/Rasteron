#include <string.h>

#include "Loader.h"

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

    if(! strcmp(extensionTarget, extension_png)) return IMG_Png;
    else if(! strcmp(extensionTarget, extension_tiff)) return IMG_Tiff;
    else if(! strcmp(extensionTarget, extension_bmp)) return IMG_Bmp;
    else return IMG_NonValid;
}

void loadFileImage(const char* fileName, FileImage* image){
    enum IMG_FileFormat format = getFormat(fileName);
    
    switch(format){
#ifdef USE_IMG_PNG
	case IMG_Png: loadFileImage_PNG(fileName, image); break;
#endif
#ifdef USE_IMG_TIFF
	case IMG_Tiff: loadFileImage_TIFF(fileName, image); break;
#endif
#ifdef USE_IMG_BMP
	case IMG_Bmp: loadFileImage_BMP(fileName, image);break;
#endif
	default: perror("Image Format not supported!"); break;
    }
}

void writeFileImageRaw(const char* fileName, enum IMG_FileFormat format, unsigned height, unsigned width, unsigned* data){
    switch(format){
#ifdef USE_IMG_PNG
	case IMG_Png: writeFileImageRaw_PNG(fileName, height, width, data); break;
#endif
#ifdef USE_IMG_TIFF
	case IMG_Tiff: writeFileImageRaw_TIFF(fileName, height, width, data); break;
#endif
#ifdef USE_IMG_BMP
	case IMG_Bmp: writeFileImageRaw_BMP(fileName, height, width, data); break;
#endif
	default: perror("Image Format not supported!"); break;
    }
}

void delFileImage(FileImage* image) {
	switch (image->fileFormat) {
#ifdef USE_IMG_PNG
	case IMG_Png: delFileImage_PNG(image); break;
#endif
#ifdef USE_IMG_TIFF
	case IMG_Tiff: delFileImage_TIFF(image); break;
#endif
#ifdef USE_IMG_BMP
	case IMG_Bmp: delFileImage_BMP(image); break;
#endif
	default: perror("Image Format not supported!"); break;
	}
}