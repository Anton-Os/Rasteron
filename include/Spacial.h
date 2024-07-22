#ifndef RASTERON_SPACE_H

#include "type_def.h"

// --------------------------------   Bounds    -------------------------------- //

typedef struct {
    float topLeft[2]; 
    float topRight[2];
    float botLeft[2]; 
    float botRight[2];
} Bounds;

Bounds createBounds(unsigned height, unsigned width);
float bounds_getHeight(Bounds bounds);
float bounds_getWidth(Bounds bounds);

// --------------------------------   Sprite    -------------------------------- //

#define SPRITE_SCALE 1000

typedef struct {
    Bounds bounds;
    const Rasteron_Image* image;
} Rasteron_Sprite;

Rasteron_Sprite* loadSprite(const Rasteron_Image* refImage);

void internal_dealloc_sprite(Rasteron_Sprite* sprite);
#define RASTERON_SPRITE_DEALLOC(sprite) (internal_dealloc_sprite(sprite))


// --------------------------------   Heightmap    -------------------------------- //

#define HEIGHTMAP_STEP 0.001 // 1 / 1000

typedef struct {
    Bounds bounds;
    float minDepth; float maxDepth;
    float* data;
} Rasteron_Heightmap;

Rasteron_Heightmap* internal_alloc_heightmap(uint32_t height, uint32_t width, double minDepth, double maxDepth);
#define RASTERON_HEIGHTMAP_ALLOC(height, width, minDepth, maxDepth) (internal_alloc_heightmap(height, width, minDepth, maxDepth))
Rasteron_Heightmap* loadHeightmap(ref_image_t refImage); // create heightmap from an image file

void internal_dealloc_heightmap(Rasteron_Heightmap* heightmap);
#define RASTERON_HEIGHTMAP_DEALLOC(heightmap) (internal_dealloc_heightmap(heightmap))

#define RASTERON_SPACE_H
#endif