#ifndef RASTERON_PRIMITIVE_H

#define MAX_COLOR_SEEDS TWOPOWER(16) // 262144
// #define DEFAULT_SEED_TOTAL 0.5 // leaves half the image in its original state
#define DEFAULT_SEED_WEIGHT 0.0

#include "Image.h"

// Sprite | For storing image within a bounding box

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

// Palette | For storing a table of colors in the quantities

#define MAX_COLOR_TABLE_VALS TWOPOWER(16) // 262144
#define DEFAULT_PALETTE_FILTER TWOPOWER(10) // 1024
#define COLOR_OFFSET 0
#define COLOR_COUNT_OFFSET 1

typedef struct {
    unsigned colors[MAX_COLOR_TABLE_VALS][2];  // 2D array of color code and color count
    unsigned colorsFound; // Increments as more colors are found
} Rasteron_Palette;

// Rasteron_Palette* createFixedPalette(const uint32_t* colorspointr, uint16_t colorCount); // Creates a palette based on a fixed list of values
Rasteron_Palette* createPalette(const Rasteron_Image* refImage); // creates a palette based on an existing image
Rasteron_Palette* filterPalette(unsigned minColorCount, const Rasteron_Palette* refPalette); // filters a palette to a more limited subset
void deletePalette(Rasteron_Palette* palette);

#define RASTERON_PRIMITIVE_H
#endif