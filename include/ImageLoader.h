#include "ImageSupport.h"

typedef struct _Image Image;

#define BAD_COLOR_CODE 0xFF000000

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
	png_byte** rowPtrs;
	uint32_t* rgbaData;
} ImageData_Png;

void loadImage_PNG(const char* fileName, Image* image);
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
void delImage_BMP(Image* image);
#endif // USE_IMG_BMP


// Issues with TARGA, not the biggest fan
#ifdef USE_IMG_TARGA
typedef struct {
	int8_t idfield;
	int16_t height;
	int16_t width;
	int8_t depth;

	int32_t* data;
} ImageData_Targa;

void loadImage_Targa(const char* fileName, Image* image);
void delImage_Targa(Image* image);
#endif // USE_IMG_TARGA

// CUSTOM IMAGE STRUCTURE

typedef struct {
	uint32_t width;
	uint32_t height;

	uint8_t count; 
	uint32_t** rasters; // For animated/layered image support
}


// Generalized image structures

enum IMG_FileFormat {
	IMG_NonValid = 0,
	IMG_Tiff = 1,
	IMG_Png = 2,
	IMG_Bmp = 3,
	IMG_Targa = 4,
	IMG_Custom = 5
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
	#ifdef USE_IMG_TARGA // DEFECTIVE!
		ImageData_Targa targa;
	#endif
};

typedef struct _Image {
	enum IMG_FileFormat fileFormat;
	union ImageData imageData;
} Image;


// General purpose functions
void revrsColorEncoding(uint32_t* raster, unsigned int pCount);// Utility Function
void makeSolidColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal);

void createImage(const char* fileName, Image* image);


#ifdef _WIN32 
	#ifndef WIN_INCLUDE
		#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
			#include <Windows.h>
		#endif
		#define WIN_INCLUDE
	#endif

	BITMAP createWinBmap(const Image* image);
	void drawWinBmap(HWND hwnd, const BITMAP* bmap);
	// void drawWinBmap(HWND hwnd, const Image* image);
#endif // Port later to some Internal.h header