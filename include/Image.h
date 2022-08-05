#ifndef RASTERON_IMAGE_H

#include "Loader.h"

/* enum SPECTRUM_Level {
	SPECTRUM_Dark, // 0% to 20% // BLACK
	SPECTRUM_Low, // 20% to 40%
	SPECTRUM_Medium, // 40% to 60%
	SPECTRUM_High, // 60% to 80%
	SPECTRUM_Bright // 80% to 100% // WHITE
}; */

// SeedTable operations

typedef struct {
	uint32_t color; // = ZERO_COLOR;
	double weight; // = DEFAULT_SEED_WEIGHT; // adjusted automatically inside calling function
} Rasteron_Seed;

#define MAX_COLOR_SEEDS TWOPOWER(11) // 2046
#define DEFAULT_SEED_WEIGHT 0.0

typedef struct {
    Rasteron_Seed seeds[MAX_COLOR_SEEDS];
	unsigned seedCount; // = 0;
} Rasteron_SeedTable;

void addSeed(Rasteron_SeedTable* table, unsigned color);
void addWeightedSeed(Rasteron_SeedTable* table, unsigned color, double weight);

// Rasteron operations

#define SWATCH_COUNT 8

enum SWATCH_Colors {
    SWATCH_Yellow_Add = 0,
    SWATCH_Cyan_Add = 1,
    SWATCH_Magenta_Add = 2,
    SWATCH_Light = 3,
    SWATCH_Dark = 4,
    SWATCH_Yellow_Sub = 5,
    SWATCH_Cyan_Sub = 6,
    SWATCH_Magenta_Sub = 7,
};

typedef struct {
    unsigned base; // base color to generate swatch
    unsigned colors[SWATCH_COUNT]; // generated swatch colors
    uint8_t deviation;
} Rasteron_Swatch;

Rasteron_Swatch createSwatch(unsigned color, uint8_t deviation);
Rasteron_SeedTable createSeedTable(const Rasteron_Swatch* swatch);

// Image operations

enum FLIP_Type { FLIP_Clock, FLIP_CounterClock, FLIP_Upside };

typedef struct {
	uint32_t height;
	uint32_t width;
} ImageSize;

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

Rasteron_Image* allocNewImg(const char* name, uint32_t height, uint32_t width); // Image allocation helper

Rasteron_Image* createImgRef(const char* fileName); // creates an image from a file
Rasteron_Image* createImgSolid(ImageSize size, uint32_t solidColor); // creates a solid color image
// Rasteron_Image* createImgCopy(Rasteron_Image* refImage); // creates copy of reference image
Rasteron_Image* createImgFlip(const Rasteron_Image* refImage, enum FLIP_Type flip); // creates flipped version of reference image

Rasteron_Image* createImgGrey(const Rasteron_Image* refImage); // creates greyscale version of reference image
Rasteron_Image* createImgFltChan(const Rasteron_Image* refImage, CHANNEL_Type channel); // creates channel-filtered version of reference image
Rasteron_Image* createImgAvgChan(const Rasteron_Image* refImage, CHANNEL_Type channel); // creates channel-averaged version of reference image

Rasteron_Image* createImgBlend(const Rasteron_Image* image1, const Rasteron_Image* image2); // creates blended version of image
Rasteron_Image* createImgFuse(const Rasteron_Image* image1, const Rasteron_Image* image2); // creates fused version of image
Rasteron_Image* createImgSeedRaw(const Rasteron_Image* refImage, uint32_t color, double prob); // seeds pixel over image based on probability
Rasteron_Image* createImgSeedWeighted(const Rasteron_Image* refImage, const Rasteron_SeedTable* seedTable); // seeds pixels over image based on weights

void deleteImg(Rasteron_Image* image);

// PixelPoint operations

#define MAX_PIXEL_POS TWOPOWER(11) // 2046

typedef struct {
	double xFrac; // fraction of image along the x axis
	double yFrac; // fraction of image along the y axis
} PixelPoint;

typedef struct {
	PixelPoint points[MAX_PIXEL_POS];
	unsigned pointCount; // = 0;
} PixelPointTable;

unsigned getPixOffset(PixelPoint pixPos, const Rasteron_Image* refImage); // gets pixel offset from image corner
unsigned getPixCursorOffset(PixelPoint pixPos, const Rasteron_Image* refImage); // gets pixel offset from cursor window position

#define RASTERON_IMAGE_H
#endif