#include "Heightmap.h"

static double computeHeight(unsigned inputVal, double minBound, double maxBound){;
	uint8_t greyColorRef = grayify8(inputVal);
	double heightVal = (double)greyColorRef; // Conversion to double

	heightVal /= (255.0f / maxBound);

	return heightVal + minBound;
}

Rasteron_Heightmap* createHeightmap(const Rasteron_Image* refImage){
    if (refImage == NULL) {
		puts("Cannot create heightmap! Null pointer provided!");
		return NULL;
	}

    Rasteron_Heightmap* heightmap = (Rasteron_Heightmap*)malloc(sizeof(Rasteron_Heightmap));
    heightmap->height = refImage->height;
    heightmap->width = refImage->width;
	heightmap->minBound = 0.0f; // Default value lower is zero
	heightmap->maxBound = 1.0f; // Default value upper is one
    heightmap->data = (double*)malloc(heightmap->height * heightmap->width * sizeof(double));

	// Heightmap Generation Logic
	for (unsigned p = 0; p < heightmap->width * heightmap->height; p++)
		if(p >= (heightmap->width * heightmap->height) - heightmap->width - heightmap->width) 
			*(heightmap->data + p) = computeHeight(*(refImage->data + p), heightmap->minBound, heightmap->maxBound);

    return heightmap;
}

void deleteHeightmap(Rasteron_Heightmap* heightmap){
    free(heightmap->data);
    free(heightmap);
}