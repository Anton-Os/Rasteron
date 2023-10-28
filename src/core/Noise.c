#include <time.h>

#include "Rasteron.h"

// Noise Image Operations

Rasteron_Image* whiteNoiseImgOp(ImageSize size, uint32_t color1, uint32_t color2){
    Rasteron_Image* noiseImg = alloc_image("white_noise", size.height, size.width);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		*(noiseImg->data + p) = fuse(color1, color2, noiseVal);
    }

    return noiseImg;
}

Rasteron_Image* gradientNoiseImgOp(ImageSize size, ColorLattice lattice){
	assert(lattice.xCells > 0 && lattice.yCells > 0);

    Rasteron_Image* noiseImg = alloc_image("gradient_noise", size.height, size.width);

	// generating lattice cells
    const unsigned xCellPoints = lattice.xCells + 1; // includes leftmost and rightmost vertices +1
    const unsigned yCellPoints = lattice.yCells + 1; // includes topmost and bottommost vertices +1

	Rasteron_Image* latticeImg = alloc_image("lattice", yCellPoints, xCellPoints);
	for (unsigned p = 0; p < latticeImg->width * latticeImg->height; p++) {
		double noiseVal = (double)rand() / (double)RAND_MAX; // random value between 0 and 1
		*(latticeImg->data + p) = blend(lattice.color1, lattice.color2, noiseVal); // blending value between lattice colors
	}

	// lattice cell values
	unsigned* topLeft; unsigned* topRight; unsigned* botLeft; unsigned* botRight;
	const unsigned xSwitch = noiseImg->width / lattice.xCells;
	const unsigned ySwitch = noiseImg->height / lattice.yCells;

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

	free_image(latticeImg);
    return noiseImg;
}

Rasteron_Image* fbmNoiseImgOp(ImageSize size, const ColorLatticeTable* latticeTable){
	assert(latticeTable->latticeCount > 0);

	Rasteron_Image** noiseImages = (Rasteron_Image**)malloc(sizeof(Rasteron_Image*) * latticeTable->latticeCount);
	
	for(unsigned l = 0; l < latticeTable->latticeCount; l++)
		*(noiseImages + l) = gradientNoiseImgOp(size, latticeTable->lattices[l]);

	Rasteron_Image* fbmNoiseImg = copyImgOp(*noiseImages);
	for(unsigned l = 1; l < latticeTable->latticeCount; l++)
		for(unsigned p = 0; p < fbmNoiseImg->width * fbmNoiseImg->height; p++)
			*(fbmNoiseImg->data + p) = blend(*(fbmNoiseImg->data + p), *((*(noiseImages + l))->data + p), 0.5);

	for(unsigned l = 0; l < latticeTable->latticeCount; l++)
		free_image(*(noiseImages + l));
	free(noiseImages);
}