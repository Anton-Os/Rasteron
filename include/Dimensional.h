#ifndef RASTERON_PRIMITIVE_H

#define MAX_COLOR_SEEDS TWOPOWER(16) // 262144
// #define DEFAULT_SEED_TOTAL 0.5 // leaves half the image in its original state
#define DEFAULT_SEED_WEIGHT 0.0

#include "Image.h"

// Sprite

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

// Heightmap

#define MAX_HEIGHTMAP_VALS TWOPOWER(16) // 262144

typedef struct {
    uint32_t width;
	uint32_t height;
    
    double minBound;
    double maxBound;
    double* data;
} Rasteron_Heightmap;

Rasteron_Heightmap* allocNewHeightmap(uint32_t height, uint32_t width, double minBound, double maxBound);
Rasteron_Heightmap* createHeightmap(const Rasteron_Image* ref); // create heightmap from an image file

void deleteHeightmap(Rasteron_Heightmap* heightmap);

#define RASTERON_PRIMITIVE_H
#endif