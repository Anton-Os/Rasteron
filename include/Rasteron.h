#ifndef RASTERON_H

#include "Loader.h"
#include "Toolbox.h"

#ifdef __cplusplus
extern "C"{
#endif

// Image Generation Functions, see ImageGen.c

// Rasteron Image

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

Rasteron_Image* createImgBase(const Image* image); // Creates an image based on target image format
Rasteron_Image* createImgGrey(const Rasteron_Image* refImage); // Creates greyscale version of reference image
Rasteron_Image* createImgFilter(const Rasteron_Image* refImage, CHANNEL_Type channel); // Creates single channel version of reference image

void deleteImg(Rasteron_Image* rast_image);

// Image Writing Functions, see ImgTIFF.c, ImgPng.c, and ImgBmp.c

#ifdef USE_IMG_TIFF
void writeImage_TIFF(const char* fileName, Rasteron_Image* image);
#endif
#ifdef USE_IMG_PNG
void writeImage_PNG(const char* fileName, Rasteron_Image* image);
#endif
#ifdef USE_IMG_BMP
void writeImage_BMP(const char* fileName, Rasteron_Image* image);
#endif

// Primitive Generation Functions, see PrimitiveGen.c

// Sprite

#define SPRITE_DIM_RATIO 1000 // dimentions used for conversion to floating points

typedef struct {
    float topLeft_Pt[2];
    float topRight_Pt[2];
    float botLeft_Pt[2];
    float botRight_Pt[2];
} Rasteron_BoxBounds;

typedef struct {
    const Rasteron_Image* image;
    Rasteron_BoxBounds bounds;
} Rasteron_Sprite;

Rasteron_Sprite* createSprite(const Rasteron_Image* refImage);
void deleteSprite(Rasteron_Sprite* sprite);

// Palette

#define MAX_COLOR_TABLE_VALS 1048576 // 2 ^ 20
#define MIN_COLOR_PIXEL_COUNTS 1024 // 2 ^ 10
#define COLOR_CODE_OFFSET 0
#define COLOR_COUNT_OFFSET 1

typedef struct {
    unsigned colors[MAX_COLOR_TABLE_VALS][2];  // 2D array of colors and cooresponding color count
    unsigned maxColors; // Increments as more colors are found
    unsigned bkIndex; // The index holding the background color
} Rasteron_Palette;

Rasteron_Palette* createPalette(const Rasteron_Image* refImage);
Rasteron_Palette* filterPalette(const Rasteron_Palette* refImage);
void deletePalette(Rasteron_Palette* palette);

#ifdef __cplusplus
}
#endif

#define RASTERON_H
#endif