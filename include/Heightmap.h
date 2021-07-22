#include "Rasteron.h"

#define MAX_HEIGHTMAP_VALS 1048576 // 2 ^ 20

typedef struct {
    uint32_t width;
	uint32_t height;
    
    double* data;
    double minBound;
    double maxBound;
} Rasteron_Heightmap;

Rasteron_Heightmap* createHeightmap(const Rasteron_Image* ref);
void deleteHeightmap(Rasteron_Heightmap* rast_heightmap);