#ifndef RASTERON_IMAGE_H

#include "Loader.h"
#include "Toolbox.h"

#define MAX_COLOR_SEEDS TWOPOWER(11) // 2046
#define DEFAULT_SEED_WEIGHT 0.0

typedef struct {
	uint32_t color; // = BAD_COLOR_CODE;
	double weight; // = DEFAULT_SEED_WEIGHT; // adjusted automatically inside calling function
} Rasteron_Seed;

typedef struct {
    Rasteron_Seed seeds[MAX_COLOR_SEEDS];
	unsigned seedCount; // = 0;
} Rasteron_SeedTable;

// Rasteron Pixel Points

typedef struct {
    double xFrac; // fraction of image along the x axis
    double yFrac; // fraction of image along the y axis
} Rasteron_PixelPoint;

typedef struct {
    Rasteron_PixelPoint pos;
    unsigned color;
} Rasteron_ColorPoint;

#define MAX_PIXEL_POS TWOPOWER(11) // 2046

typedef struct {
    Rasteron_PixelPoint positions[MAX_PIXEL_POS];
    unsigned pixelPointCount; // = 0;
} Rasteron_PixelPointTable;

typedef struct {
    Rasteron_ColorPoint positions[MAX_PIXEL_POS];
    unsigned pixelPointCount; // = 0;
} Rasteron_ColorPointTable;

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
Rasteron_Image* createImgProxCell(const Rasteron_Image* refImage, const Rasteron_ColorPointTable* colorPointTable); // Creates a cell layout based on closest points

void deleteImg(Rasteron_Image* rast_image);

void addColorToSeeds(Rasteron_SeedTable* seeds, uint32_t color, double weight);
unsigned getPixIndexFromPos(Rasteron_PixelPoint* pixPos, Rasteron_Image* refImage);

#define RASTERON_IMAGE_H
#endif