#include "Heightmap.h"

static double computeHeight(unsigned inputVal, double minBound, double maxBound){
	uint32_t maxVal = 0x00FFFFFF;
	double maxClampVal = (double)maxVal / maxBound;

	uint32_t greyColor = grayify_32((uint32_t)inputVal);
	greyColor = greyColor & maxVal; // Sets the alpha value to zero to simplify computation

	double testVal;
	if(inputVal != 0)
		testVal = ((double)greyColor / maxClampVal) + minBound;

	return ((double)greyColor / maxClampVal) + minBound;
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

	for (unsigned p = 0; p < rstn_heightmap->width * rstn_heightmap->height; p++)
		*(rstn_heightmap->data + p) = computeHeight(*(ref->data + p), rstn_heightmap->minBound, rstn_heightmap->maxBound);

    return rstn_heightmap;
}

void rstnDel_Heightmap(Rasteron_Heightmap* rstn_heightmap){
    free(rstn_heightmap->data);
    free(rstn_heightmap);
}