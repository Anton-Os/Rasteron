#include "type.h"

// --------------------------------   Space    -------------------------------- //

#define SPACE_SCALE 1000

typedef struct {
    float topLeft[2]; 
    float topRight[2];
    float botLeft[2]; 
    float botRight[2];
} SpaceBounds;

SpaceBounds createSpaceBounds(unsigned height, unsigned width);

// --------------------------------   Sprite    -------------------------------- //

typedef struct {
    SpaceBounds bounds;
    const Rasteron_Image* image;
} Rasteron_Sprite;

Rasteron_Sprite* loadSprite(const Rasteron_Image* refImage);
void dealloc_sprite(Rasteron_Sprite* sprite);


// --------------------------------   Heightmap    -------------------------------- //

#define HEIGHTMAP_STEP 0.001 // 1 / 1000

typedef struct {
    SpaceBounds bounds;
    float minDepth; float maxDepth;
    float* data;
} Rasteron_Heightmap;

Rasteron_Heightmap* alloc_heightmap(uint32_t height, uint32_t width, double minDepth, double maxDepth);
Rasteron_Heightmap* loadHeightmap(ref_image_t refImage); // create heightmap from an image file
void dealloc_heightmap(Rasteron_Heightmap* heightmap);