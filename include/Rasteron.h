#ifndef RASTERON_H

#include "Loader.h"
#include "Toolbox.h"

#ifdef __cplusplus
extern "C"{
#endif

// Loader.c declarations

typedef Image FileImage; // Just makes an easier distinction between Rasteron Image and file Image

void rstnLoadFromFile(const char* fileName, FileImage* image); // Image is destination
void rstnDelFromFile(FileImage* image);

// Primitives.c declarations
// Rasteron specific image type, based on file image with a name (label) attached

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

Rasteron_Image* createImgBase(const Image* image); // Creates an image based on my readers
Rasteron_Image* createImgGrey(const Rasteron_Image* refImage); // Creates greyscale version of target image

void deleteImg(Rasteron_Image* rstn_image);

// Sprite is a wrapper around a base image and coordinates

#define DIMENSION_RATIO_UNSIGNED_TO_FLOAT 1000

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

// Palette holds the frequencies of colors that appear in an image

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