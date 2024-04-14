#include "Rasteron.h"

#include "Spacial.h"

// --------------------------------   Space    -------------------------------- //

Bounds createBounds(unsigned height, unsigned width){
	Bounds bounds;

	bounds.topRight[0] = width / 2.0f / SPRITE_SCALE;
	bounds.topRight[1] = height / 2.0f / SPRITE_SCALE;
	bounds.topLeft[0] = -1.0 * (width / 2.0f / SPRITE_SCALE);
	bounds.topLeft[1] = height / 2.0f / SPRITE_SCALE;
	bounds.botRight[0] = width / 2.0f / SPRITE_SCALE;
	bounds.botRight[1] = -1.0 * (height / 2.0f / SPRITE_SCALE);
	bounds.botLeft[0] = -1.0 * (width / 2.0f / SPRITE_SCALE);
	bounds.botLeft[1] = -1.0 * (height / 2.0f / SPRITE_SCALE);

	return bounds;
}

float bounds_getHeight(Bounds bounds){ return !(_invertImage)? bounds.topRight[1] - bounds.botRight[1] : bounds.topRight[0] - bounds.botLeft[0]; }
float bounds_getWidth(Bounds bounds){ return !(_invertImage)? bounds.topRight[0] - bounds.botLeft[0] : bounds.topRight[1] - bounds.botRight[1]; }

// --------------------------------   Sprite    -------------------------------- //

Rasteron_Sprite* loadSprite(const Rasteron_Image* refImage){
	assert(refImage != NULL);

	Rasteron_Sprite* sprite = (Rasteron_Sprite*)malloc(sizeof(Rasteron_Sprite));

	sprite->bounds = createBounds(refImage->height, refImage->width);
	sprite->image = refImage; // store a const pointer to the image

	return sprite;
}

void internal_dealloc_sprite(Rasteron_Sprite* sprite){
	if(sprite->image != NULL) RASTERON_DEALLOC(sprite->image);
	if(sprite != NULL) free(sprite);
	sprite = NULL;
}

// --------------------------------   Heightmap    -------------------------------- //

static float calcHeight(unsigned inputColor, float minDepth, float maxDepth){;
	uint8_t greyColorRef = grayify8(inputColor);
	float heightVal = (float)greyColorRef; // conversion to double
	heightVal /= (255.0 / maxDepth);

	return heightVal + minDepth;
}

Rasteron_Heightmap* internal_alloc_heightmap(uint32_t height, uint32_t width, double minDepth, double maxDepth){
	Rasteron_Heightmap* heightmap = (Rasteron_Heightmap*)malloc(sizeof(Rasteron_Heightmap));
    
	heightmap->bounds = createBounds(height, width);
	heightmap->minDepth = minDepth; // default min is zero
	heightmap->maxDepth = maxDepth; // default max is one
    heightmap->data = (double*)malloc(height * width * sizeof(double));

	return heightmap;
}

Rasteron_Heightmap* loadHeightmap(ref_image_t refImage){
	assert(refImage != NULL);

    Rasteron_Heightmap* heightmap = internal_alloc_heightmap(refImage->width, refImage->height, 0.0, 1.0);

	for (unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(heightmap->data + p) = calcHeight(*(refImage->data + p), heightmap->minDepth, heightmap->maxDepth);
	
    return heightmap;
}

void internal_dealloc_heightmap(Rasteron_Heightmap* heightmap){
	if(heightmap->data != NULL) free(heightmap->data);
    if(heightmap != NULL) free(heightmap);
	heightmap = NULL;
}