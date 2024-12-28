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

void loadFromFile(const char* fileName, FileImage* image){
    enum IMG_FileFormat format = getFormat(fileName);
#ifdef _WIN32
    replaceFwdSlash(fileName);
    if(_access(fileName, 0)){
#elif defined(__linux__)
    if(access(fileName, F_OK) != 0){ // image not loaded successfully
#endif

        image->fileFormat = IMG_NonValid;
        return;
    }

    switch(format){
#ifdef USE_IMG_PNG
	case IMG_Png: loadFromFile_PNG(fileName, image); break;
#endif
#ifdef USE_IMG_TIFF
	case IMG_Tiff: loadFromFile_TIFF(fileName, image); break;
#endif
#ifdef USE_IMG_BMP
	case IMG_Bmp: loadFromFile_BMP(fileName, image);break;
#endif
	default:
        printf("Unsupported File Type!");
        image = NULL;
        break;
    }
}

void writeFileImageRaw(const char* fileName, enum IMG_FileFormat format, unsigned height, unsigned width, unsigned* data){
    switch(format){
#ifdef USE_IMG_PNG
	case IMG_Png:
        // if(!strcmp(fileName, "png")) strcat(fileName, ".png");
        writeFileImageRaw_PNG(fileName, height, width, data);
        break;
#endif
#ifdef USE_IMG_TIFF
	case IMG_Tiff:
        // if(!strcmp(fileName, "iff") || !strcmp(fileName, "tif")) strcat(fileName, ".tiff");
        writeFileImageRaw_TIFF(fileName, height, width, data);
        break;
#endif
#ifdef USE_IMG_BMP
	case IMG_Bmp:
        // if(!strcmp(fileName, "bmp")) strcat(fileName, ".bmp");
        writeFileImageRaw_BMP(fileName, height, width, data);
        break;
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
