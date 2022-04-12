#include <time.h>

#include "Noise.h"

// Noise Image Operations

Rasteron_Image* createNoiseImg_white(const Rasteron_Image* refImage, uint32_t color1, uint32_t color2){
    if (refImage == NULL) {
		perror("Cannot create white noise! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* noiseImg = allocNewImg("noise-w", refImage->height, refImage->width);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		*(noiseImg->data + p) = fuse(color1, color2, noiseVal);
    }

    return noiseImg;
}

Rasteron_Image* createNoiseImg_gradient(const Rasteron_Image* refImage, GradientLattice lattice){
	if (lattice.xCellDivs == 0 || lattice.yCellDivs == 0 || refImage == NULL) {
        perror("Cannot create gradient noise! Invalid parameters!");
        return NULL;
    }

    Rasteron_Image* noiseImg = allocNewImg("noise-g", refImage->height, refImage->width);

	// generating lattice cells
    const unsigned xCellPoints = lattice.xCellDivs + 1; // includes leftmost and rightmost vertices +1
    const unsigned yCellPoints = lattice.yCellDivs + 1; // includes topmost and bottommost vertices +1

	Rasteron_Image* latticeImg = allocNewImg("lattice", yCellPoints, xCellPoints);
	for (unsigned p = 0; p < latticeImg->width * latticeImg->height; p++) {
		double noiseVal = (double)rand() / (double)RAND_MAX; // random value between 0 and 1
		*(latticeImg->data + p) = blend(lattice.color1, lattice.color2, noiseVal); // blending value between lattice colors
	}

	// lattice cell values
	unsigned* topLeft; unsigned* topRight; unsigned* botLeft; unsigned* botRight;
	const unsigned xSwitch = noiseImg->width / lattice.xCellDivs;
	const unsigned ySwitch = noiseImg->height / lattice.yCellDivs;

    for(unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        unsigned xOffset = p % noiseImg->width; // absolute X pixel offset
        unsigned yOffset = p / noiseImg->width; // absolute Y pixel offset
		
		// setting lattice cell values
		if(xOffset == 0){ // repositions to correct row inside lattice
			unsigned short row = (yOffset / ySwitch) * latticeImg->width;

			topLeft = latticeImg->data + row;
			topRight = latticeImg->data + 1 + row;
			botLeft = latticeImg->data + latticeImg->width + row;
			botRight = latticeImg->data + latticeImg->width + 1 + row;
		}
		else if (xOffset % xSwitch == 0 && p > 0) { // increment to next column inside lattice
			topLeft++; topRight++; botLeft++; botRight++;
		}

		// color determination
		double xFrac = (double)(xOffset % xSwitch) / (double)xSwitch; // relative X offset inside lattice cell
		double yFrac = (double)(yOffset % ySwitch) / (double)ySwitch; // relative Y offset inside lattice cell

		unsigned newColor = blend(
			blend(*topLeft, *topRight, xFrac),
			blend(*botLeft, *botRight, xFrac),
			yFrac
		);
        
        *(noiseImg->data + p) = newColor;
    }

	deleteImg(latticeImg);
    return noiseImg;
}