#include "Rasteron.h"

#define MAX_HEIGHTMAP_VALS 1048576 // 2 ^ 20

typedef struct {
    uint32_t width;
	uint32_t height;
    
    double minBound;
    double maxBound;
    double* data;
} Rasteron_Heightmap;

Rasteron_Heightmap* allocNewLattice(uint32_t width, uint32_t height, double minBound, double maxBound);
Rasteron_Heightmap* createLattice(const Rasteron_Image* ref); // create lattice from an image file

void deleteLattice(Rasteron_Heightmap* lattice);