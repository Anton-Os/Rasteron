#include "Lattice.h"

static double computeHeight(unsigned inputColor, double minBound, double maxBound){;
	uint8_t greyColorRef = grayify8(inputColor);
	double heightVal = (double)greyColorRef; // Conversion to double
	heightVal /= (255.0 / maxBound);

	return heightVal + minBound;
}

Rasteron_Lattice* allocNewLattice(uint32_t width, uint32_t height, double minBound, double maxBound){
	Rasteron_Lattice* lattice = (Rasteron_Lattice*)malloc(sizeof(Rasteron_Lattice));
    lattice->height = height;
    lattice->width = width;
	lattice->minBound = minBound; // Default value lower is zero
	lattice->maxBound = maxBound; // Default value upper is one
    lattice->data = (double*)malloc(lattice->height * lattice->width * sizeof(double));

	return lattice;
}

Rasteron_Lattice* createLattice(const Rasteron_Image* refImage){
    if (refImage == NULL) {
		puts("Cannot create lattice! Null pointer provided!");
		return NULL;
	}

    Rasteron_Lattice* lattice = allocNewLattice(refImage->width, refImage->height, 0.0, 1.0);

	// Lattice Generation Logic
	for (unsigned p = 0; p < lattice->width * lattice->height; p++)
		*(lattice->data + p) = computeHeight(*(refImage->data + p), lattice->minBound, lattice->maxBound);
		// if(p >= (lattice->width * lattice->height) - lattice->width - lattice->width) 

    return lattice;
}

void deleteLattice(Rasteron_Lattice* lattice){
    if(lattice->data != NULL) free(lattice->data);
    if(lattice != NULL) free(lattice);
}