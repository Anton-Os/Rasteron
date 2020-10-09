#include "Heightmap.h"

Rasteron_Heightmap* rstnCreate_Heightmap(const Rasteron_Image* ref){
    if (ref == NULL) {
		puts("Cannot create heightmap! Null pointer provided!");
		return NULL;
	} else if(strcmp(ref->name, "grey") != 0) {
		puts("Cannot create heightmap! Target image is not greyscale");
		return NULL;
	} 

    Rasteron_Heightmap* rstn_heightmap = (Rasteron_Heightmap*)malloc(sizeof(Rasteron_Heightmap));

    rstn_heightmap->height = ref->height;
    rstn_heightmap->width = ref->width;
	rstn_heightmap->minBound = 0.0f; // Default value lower is zero
	rstn_heightmap->maxBound = 1.0f; // Default value upper is one

    rstn_heightmap->data = (double*)malloc(rstn_heightmap->height * rstn_heightmap->width * sizeof(double));

	for (unsigned p = 0; p < rstn_heightmap->width * rstn_heightmap->height; p++) {
		// TODO: Write test values here!
		*(rstn_heightmap->data + p) = (rstn_heightmap->maxBound * ((double)(*(ref->data + p) & 0xFF00))) / 255.0;
	}

    return rstn_heightmap;
}

void rstnDel_Heightmap(Rasteron_Heightmap* rstn_heightmap){
    free(rstn_heightmap->data);
    free(rstn_heightmap);
}