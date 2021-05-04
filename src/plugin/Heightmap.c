#include "Heightmap.h"

static double computeHeight(unsigned inputVal, double minBound, double maxBound){;

	uint8_t greyColorRef = grayify_8(inputVal);
	double heightVal = (double)greyColorRef; // Conversion to double

	heightVal /= (255.0f / maxBound);

	return heightVal + minBound;
}

Rasteron_Heightmap* rstnCreate_Heightmap(const Rasteron_Image* ref){
    if (ref == NULL) {
		puts("Cannot create heightmap! Null pointer provided!");
		return NULL;
	}

    Rasteron_Heightmap* rstn_heightmap = (Rasteron_Heightmap*)malloc(sizeof(Rasteron_Heightmap));

    rstn_heightmap->height = ref->height;
    rstn_heightmap->width = ref->width;
	rstn_heightmap->minBound = 0.0f; // Default value lower is zero
	rstn_heightmap->maxBound = 1.0f; // Default value upper is one

    rstn_heightmap->data = (double*)malloc(rstn_heightmap->height * rstn_heightmap->width * sizeof(double));

	// for (unsigned p = 0; p < rstn_heightmap->width * rstn_heightmap->height; p++)
		// *(rstn_heightmap->data + p) = computeHeight(*(ref->data + p), rstn_heightmap->minBound, rstn_heightmap->maxBound);
	for (unsigned p = 0; p < rstn_heightmap->width * rstn_heightmap->height; p++)
		if(p >= (rstn_heightmap->width * rstn_heightmap->height) - rstn_heightmap->width - rstn_heightmap->width) *(rstn_heightmap->data + p) = computeHeight(*(ref->data + p), rstn_heightmap->minBound, rstn_heightmap->maxBound);

    return rstn_heightmap;
}

void rstnDel_Heightmap(Rasteron_Heightmap* rstn_heightmap){
    free(rstn_heightmap->data);
    free(rstn_heightmap);
}