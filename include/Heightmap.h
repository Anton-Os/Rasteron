#include "Rasteron.h"

#define MAX_HEIGHTMAP_VALS 131072

typedef struct {
    uint32_t width;
	uint32_t height;
    
    double* data;
    double minBound;
    double maxBound;
} Rasteron_Heightmap;

Rasteron_Heightmap* rstnCreate_Heightmap(const Rasteron_Image* ref);
void rstnDel_Heightmap(Rasteron_Heightmap* rstn_heightmap);