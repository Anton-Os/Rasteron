#include "Dimensional.h"

// Sprite operations

Rasteron_Sprite* createSprite(const Rasteron_Image* refImage){
	if (refImage == NULL) {
		perror("Cannot create palette! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Sprite* sprite = (Rasteron_Sprite*)malloc(sizeof(Rasteron_Sprite));

	sprite->image = refImage; // Simply copy a pointer to the image

	sprite->bounds.topRight_point[0] = refImage->width / 2.0f / SPRITE_DIM_RATIO ;
	sprite->bounds.topRight_point[1] = refImage->height / 2.0f / SPRITE_DIM_RATIO ;

	sprite->bounds.topLeft_point[0] = -1.0 * (refImage->width / 2.0f / SPRITE_DIM_RATIO );
	sprite->bounds.topLeft_point[1] = refImage->height / 2.0f / SPRITE_DIM_RATIO ;

	sprite->bounds.botRight_point[0] = refImage->width / 2.0f / SPRITE_DIM_RATIO ;
	sprite->bounds.botRight_point[1] = -1.0 * (refImage->height / 2.0f / SPRITE_DIM_RATIO );

	sprite->bounds.botLeft_point[0] = -1.0 * (refImage->width / 2.0f / SPRITE_DIM_RATIO );
	sprite->bounds.botLeft_point[1] = -1.0 * (refImage->height / 2.0f / SPRITE_DIM_RATIO );

	return sprite;
}

void deleteSprite(Rasteron_Sprite* sprite){
	if(sprite != NULL) free(sprite);
	sprite = NULL;
}

// Heightmap operations

static double computeHeight(unsigned inputColor, double minBound, double maxBound){;
	uint8_t greyColorRef = grayify8(inputColor);
	double heightVal = (double)greyColorRef; // Conversion to double
	heightVal /= (255.0 / maxBound);

	return heightVal + minBound;
}

Rasteron_Heightmap* allocNewHeightmap(uint32_t height, uint32_t width, double minBound, double maxBound){
	Rasteron_Heightmap* heightmap = (Rasteron_Heightmap*)malloc(sizeof(Rasteron_Heightmap));
    heightmap->height = height;
    heightmap->width = width;
	heightmap->minBound = minBound; // Default value lower is zero
	heightmap->maxBound = maxBound; // Default value upper is one
    heightmap->data = (double*)malloc(heightmap->height * heightmap->width * sizeof(double));

	return heightmap;
}

Rasteron_Heightmap* createHeightmap(const Rasteron_Image* refImage){
    if (refImage == NULL) {
		perror("Cannot create heightmap! Null pointer provided!");
		return NULL;
	}

    Rasteron_Heightmap* heightmap = allocNewHeightmap(refImage->width, refImage->height, 0.0, 1.0);

	// Heightmap Generation Logic
	for (unsigned p = 0; p < heightmap->width * heightmap->height; p++)
		*(heightmap->data + p) = computeHeight(*(refImage->data + p), heightmap->minBound, heightmap->maxBound);

    return heightmap;
}

void deleteHeightmap(Rasteron_Heightmap* heightmap){
    if(heightmap->data != NULL) free(heightmap->data);
    if(heightmap != NULL) free(heightmap);
	heightmap = NULL;
}