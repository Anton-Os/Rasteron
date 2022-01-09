#include "Heightmap.h"

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
		puts("Cannot create heightmap! Null pointer provided!");
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
}