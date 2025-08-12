#ifndef IMAGE_LOADER_H

#include "support_def.h"
#include "type_def.h"

#ifdef __linux__
#include "unistd.h"
#endif

#ifdef USE_IMG_TIFF
#include <tiffio.h>
#endif // USE_IMG_TIFF
#ifdef USE_IMG_PNG
#include <png.h>
#endif // USE_IMG_PNG

enum IMG_FileFormat { 
	IMG_NonValid = 0, 
	IMG_Tiff = 1, 
	IMG_Png = 2, 
	IMG_Bmp = 3, 
};

// Generic Loader Functions

DllExport enum IMG_FileFormat getFormat(const char* fileName);
DllExport Rasteron_Image* loadImgOp(const char* fileName); // creates an image from a file
DllExport void writeFileImageRaw(const char* fileName, enum IMG_FileFormat format, unsigned height, unsigned width, unsigned* data);

// JPEG Loader Functions
#ifdef USE_IMG_JPEG
Rasteron_Image* loadImgOp_jpeg(const char* fileName)
void writeFileImageRaw_jpeg(const char* fileName, unsigned height, unsigned width, unsigned* data);
#endif

// TIFF Loader Functions
#ifdef USE_IMG_TIFF
Rasteron_Image* loadImgOp_tiff(const char* fileName);
void writeFileImageRaw_tiff(const char* fileName, unsigned height, unsigned width, unsigned* data);
#endif

// PNG Loader Functions
#ifdef USE_IMG_PNG
Rasteron_Image* loadImgOp_png(const char* fileName);
void writeFileImageRaw_png(const char* fileName, unsigned height, unsigned width, unsigned* data);
#endif

// BMP Loader Functions
#ifdef USE_IMG_BMP
Rasteron_Image* loadImgOp_bmp(const char* fileName);
void writeFileImageRaw_bmp(const char* fileName, unsigned height, unsigned width, unsigned* data);
#endif

#define IMAGE_LOADER_H
#endif
