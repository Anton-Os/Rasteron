#include "Rasteron.h"

#define MAX_HEIGHTMAP_VALS 1048576 // 2 ^ 20

typedef struct {
    uint32_t width;
	uint32_t height;
    
    double minBound;
    double maxBound;
    double* data;
} Rasteron_Lattice;

Rasteron_Lattice* allocNewLattice(uint32_t width, uint32_t height, double minBound, double maxBound);
Rasteron_Lattice* createLattice(const Rasteron_Image* ref);

void deleteLattice(Rasteron_Lattice* lattice);