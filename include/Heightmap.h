#ifndef RASTERON_HEIGHTMAP_H

#include "Image.h"

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

void deleteHeightmap(Rasteron_Heightmap* lattice);

#define RASTERON_HEIGHTMAP_H
#endif