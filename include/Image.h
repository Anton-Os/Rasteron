#ifndef RASTERON_IMAGE_H

#include "Loader.h"
#include "Toolbox.h"

// Helper Containers for Images

#define MAX_COLOR_SEEDS TWOPOWER(11) // 2046
#define DEFAULT_SEED_WEIGHT 0.0

enum FLIP_Type {
    FLIP_Clock,
    FlIP_CounterClock,
    FLIP_Upside
};

// Rasteron Seed Structures

typedef struct {
	uint32_t color; // = ZERO_COLOR;
	double weight; // = DEFAULT_SEED_WEIGHT; // adjusted automatically inside calling function
} Rasteron_Seed;

typedef struct {
    Rasteron_Seed seeds[MAX_COLOR_SEEDS];
	unsigned seedCount; // = 0;
} Rasteron_SeedTable;

void addSeed(Rasteron_SeedTable* table, unsigned color);
void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight);

// Rasteron Pixel Point Structures

typedef struct {
    double xFrac; // fraction of image along the x axis
    double yFrac; // fraction of image along the y axis
} Rasteron_PixelPoint;

typedef struct {
    Rasteron_PixelPoint pos;
    unsigned color;
} Rasteron_ColorPoint;

#define MAX_PIXEL_POS TWOPOWER(11) // 2046

/* typedef struct {
    Rasteron_PixelPoint positions[MAX_PIXEL_POS];
    unsigned pixelPointCount; // = 0;
} Rasteron_PixelPointTable; */

typedef struct {
    Rasteron_ColorPoint positions[MAX_PIXEL_POS];
    unsigned pixelPointCount; // = 0;
} Rasteron_ColorPointTable;

void addColorPoint(Rasteron_ColorPointTable* table, unsigned color, double xFrac, double yFrac);

// Rasteron Image

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

// Image Generation Functions, see Image.c

Rasteron_Image* allocNewImg(const char* name, uint32_t height, uint32_t width); // Image allocation helper

Rasteron_Image* createImgRef(const char* fileName); // creates an image from a file
Rasteron_Image* createImgBlank(uint32_t height, uint32_t width, uint32_t solidColor); // Creates a blank image
// Rasteron_Image* createImgCopy(Rasteron_Image* refImage); // creates copy of reference image
Rasteron_Image* createImgFlip(const Rasteron_Image* refImage, enum FLIP_Type flip);

Rasteron_Image* createImgGrey(const Rasteron_Image* refImage); // creates greyscale version of reference image
Rasteron_Image* createImgFltCh(const Rasteron_Image* refImage, CHANNEL_Type channel); // creates channel-filtered version of reference image
Rasteron_Image* createImgAvgCh(const Rasteron_Image* refImage, CHANNEL_Type channel); // creates channel-averaged version of reference image

Rasteron_Image* createImgBlend(const Rasteron_Image* image1, const Rasteron_Image* image2); // creates blended version of image
Rasteron_Image* createImgBlendCh(const Rasteron_Image* image1, CHANNEL_Type channel1, const Rasteron_Image* image2, CHANNEL_Type channel2); // creates blended image from 2 channels
// Rasteron_Image* createImgInterp(const Rasteron_Image* image1, const Rasteron_Image* image2); // creates interpolated version of image
Rasteron_Image* createImgSeedRaw(const Rasteron_Image* refImage, uint32_t color, double prob); // seeds pixel over image based on probability
Rasteron_Image* createImgSeedWeighted(const Rasteron_Image* refImage, const Rasteron_SeedTable* seedTable); // seeds pixels over image based on weights
Rasteron_Image* createImgProxim(const Rasteron_Image* refImage, const Rasteron_ColorPointTable* colorPointTable); // creates a cell layout based on proximity points

void deleteImg(Rasteron_Image* image);

// Additional Functions

unsigned getPixOffset(const Rasteron_PixelPoint* pixPos, Rasteron_Image* refImage); // gets pixel offset from image corner
unsigned getPixCursorOffset(const Rasteron_PixelPoint* pixPos, Rasteron_Image* refImage); // gets pixel offset from cursor window position

#define RASTERON_IMAGE_H
#endif