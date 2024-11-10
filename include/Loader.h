#ifndef IMAGE_LOADER_H

#include "support_def.h"

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
	uint32* data;
} ImageData_Tiff;

#endif // USE_IMG_TIFF
#ifdef USE_IMG_PNG

#include <png.h>

typedef struct {
	png_uint_32 height;
	png_uint_32 width;
	int bitDepth;
	int colorType;
	size_t rowBytesCount;
	png_byte** row_ptrs;
	uint32_t* data;
} ImageData_Png;

#endif // USE_IMG_PNG
#ifdef USE_IMG_BMP

typedef struct {
	uint16_t typeCheck;
	uint32_t offset;
	int32_t height;
	int32_t width;

	uint32_t* data;
} ImageData_Bmp;

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

typedef struct {
	enum IMG_FileFormat fileFormat;
	union ImageData data;
} FileImage;

// Generic Loader Functions

void loadFromFile(const char* fileName, FileImage* image);
void writeFileImageRaw(const char* fileName, enum IMG_FileFormat format, unsigned height, unsigned width, unsigned* data);
void delFileImage(FileImage* image);

// TIFF Loader Functions
#ifdef USE_IMG_JPEG
void loadFromFile_JPEG(const char* fileName, FileImage* image);
void writeFileImageRaw_JPEG(const char* fileName, unsigned height, unsigned width, unsigned* data);
void delFileImage_JPEG(FileImage* image);
#endif

// TIFF Loader Functions
#ifdef USE_IMG_TIFF
void loadFromFile_TIFF(const char* fileName, FileImage* image);
void writeFileImageRaw_TIFF(const char* fileName, unsigned height, unsigned width, unsigned* data);
void delFileImage_TIFF(FileImage* image);
#endif

// PNG Loader Functions
#ifdef USE_IMG_PNG
void loadFromFile_PNG(const char* fileName, FileImage* image);
void writeFileImageRaw_PNG(const char* fileName, unsigned height, unsigned width, unsigned* data);
void delFileImage_PNG(FileImage* image);
#endif

// BMP Loader Functions
#ifdef USE_IMG_BMP
void loadFromFile_BMP(const char* fileName, FileImage* image);
void writeFileImageRaw_BMP(const char* fileName, unsigned height, unsigned width, unsigned* data);
void delFileImage_BMP(FileImage* image);
#endif

#define IMAGE_LOADER_H
#endif
