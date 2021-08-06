#ifndef RASTERON_H

#include "Loader.h"
#include "Toolbox.h"

#ifdef __cplusplus
extern "C"{
#endif

// Rasteron Color Seeds

#define MAX_COLOR_SEEDS TWOPOWER(11) // 2046
// #define DEFAULT_SEED_TOTAL 0.5 // leaves half the image in its original state
#define DEFAULT_SEED_WEIGHT 0.0

typedef struct {
	uint32_t color; // = BAD_COLOR_CODE;
	double weight; // = DEFAULT_SEED_WEIGHT; // adjusted automatically inside calling function
} Rasteron_Seed;

typedef struct {
    Rasteron_Seed seeds[MAX_COLOR_SEEDS];
	unsigned seedCount; // = 0;
} Rasteron_SeedTable;

// void addColorToSeeds(Rasteron_SeedTable* seeds, uint32_t color);
// void addWeightedColorToSeeds(Rasteron_SeedTable* seeds, uint32_t color, double weight);

// Rasteron Image

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

// Image Generation Functions, see ImageGen.c

Rasteron_Image* allocNewImg(const char* name, uint32_t width, uint32_t height); // Image allocation helper

Rasteron_Image* createImgBlank(uint32_t width, uint32_t height, uint32_t solidColor); // Creates a blank image
Rasteron_Image* createImgBase(const FileImage* image); // Creates an image based on target file image

Rasteron_Image* createImgGrey(const Rasteron_Image* refImage); // Creates greyscale version of reference image
Rasteron_Image* createImgFilter(const Rasteron_Image* refImage, CHANNEL_Type channel); // Creates single channel version of reference image
Rasteron_Image* createImgScatter(const Rasteron_Image* refImage, uint32_t color, double prob); // Scatters pixel over image based on probability
Rasteron_Image* createImgSplash(const Rasteron_Image* refImage, const Rasteron_SeedTable* seedTable); // Splashes colors over image based on weights 

void deleteImg(Rasteron_Image* rast_image);

// Rasteron Sprite
// Primitive Generation Functions, see PrimitiveGen.c

#define SPRITE_DIM_RATIO 1000 // dimentions used for conversion to floating points

typedef struct {
    float topLeft_point[2];
    float topRight_point[2];
    float botLeft_point[2];
    float botRight_point[2];
} Rasteron_BoxBounds;

typedef struct {
    const Rasteron_Image* image;
    Rasteron_BoxBounds bounds;
} Rasteron_Sprite;

Rasteron_Sprite* createSprite(const Rasteron_Image* refImage);
void deleteSprite(Rasteron_Sprite* sprite);

// Palette

#define MAX_COLOR_TABLE_VALS TWOPOWER(16) // 262144
#define DEFAULT_PALETTE_FILTER TWOPOWER(10) // 1024
#define COLOR_CODE_OFFSET 0
#define COLOR_COUNT_OFFSET 1

typedef struct {
    unsigned colors[MAX_COLOR_TABLE_VALS][2];  // 2D array of colors and cooresponding color count
    unsigned colorCount; // Increments as more colors are found
} Rasteron_Palette;

// Rasteron_Palette* createFixedPalette(const uint32_t* colorspointr, uint16_t colorCount); // Creates a palette based on a fixed list of values
Rasteron_Palette* createPalette(const Rasteron_Image* refImage); // Creates a palette based on an existing image
Rasteron_Palette* createLimPalette(unsigned minColorCount, const Rasteron_Palette* refPalette); // Filters a palette to a more limited subset
void deletePalette(Rasteron_Palette* palette);

#ifdef __cplusplus
}
#endif

#define RASTERON_H
#endif