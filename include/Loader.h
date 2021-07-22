#include "support.h"

#ifdef __cplusplus
extern "C"{
#endif

#ifndef IMAGE_LOADER_H

typedef struct _Image Image;

#ifdef USE_IMG_TIFF
#include <tiffio.h>

typedef struct {
	TIFF* tiff;
	uint16 compression;
	uint16 orientation;

	uint32 width;
	uint32 length;
	uint16 bitsPerSample;
	uint16 samplesPerPixel;
	uint32* raster;
} ImageData_Tiff; // TIFF-specific Data Structure

void loadImage_TIFF(const char* fileName, Image* image);
// void writeImage_TIFF(const char* fileName, Rasteron_Image* image)
void delImage_TIFF(Image* image);
#endif // USE_IMG_TIFF


#ifdef USE_IMG_PNG
#include <png.h>

typedef struct {
	png_uint_32 height;
	png_uint_32 width;
	int bitDepth;
	int colorType;
	size_t rowBytes;
	png_byte** row_ptrs;
	uint32_t* rgbaData;
} ImageData_Png;

void loadImage_PNG(const char* fileName, Image* image);
// void writeImage_PNG(const char* fileName, Rasteron_Image* image)
void delImage_PNG(Image* image);
#endif // USE_IMG_PNG


#ifdef USE_IMG_BMP
typedef struct {
	uint16_t typeCheck;
	uint32_t offset;
	int32_t height; // CAN BE NEGATIVE, always abs(height)
	int32_t width;  // CAN BE NEGATIVE, always abs(width)

	uint32_t* data;
} ImageData_Bmp;

void loadImage_BMP(const char* fileName, Image* image);
// void writeImage_BMP(const char* fileName, Rasteron_Image* image);
void delImage_BMP(Image* image);
#endif // USE_IMG_BMP


enum IMG_FileFormat {
	IMG_NonValid = 0,
	IMG_Tiff = 1,
	IMG_Png = 2,
	IMG_Bmp = 3,
};

union ImageData {
	#ifdef USE_IMG_TIFF
		ImageData_Tiff tiff;
	#endif
	#ifdef USE_IMG_PNG
		ImageData_Png png;
	#endif
	#ifdef USE_IMG_BMP
		ImageData_Bmp bmp;
	#endif
};

typedef struct _Image {
	enum IMG_FileFormat fileFormat;
	union ImageData imageData;
} Image;

/* static enum IMG_FileFormat getFileExtension(const char* filePath) {
	return IMG_NonValid;
} */

// Loader Functions, see Loader.c

typedef Image FileImage; // just makes an easier distinction between Rasteron Image and file Image

void loadFileImage(const char* fileName, FileImage* image); // image is destination
void delFileImage(FileImage* image);

#define IMAGE_LOADER_H
#endif // IMAGE_LOADER_H

#ifdef __cplusplus
}
#endif 