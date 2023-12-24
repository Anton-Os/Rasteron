#include <time.h>

#include "Rasteron.h"

// Noise Image Operations

Rasteron_Image* noiseImgOp_white(ImageSize size, uint32_t color1, uint32_t color2){
    Rasteron_Image* noiseImg = alloc_image("white_noise", size.height, size.width);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		*(noiseImg->data + p) = fuseColors(color1, color2, noiseVal);
    }

    return noiseImg;
}

Rasteron_Image* noiseImgOp_grid(ImageSize size, ColorGrid grid){
	assert(grid.xCells > 0 && grid.yCells > 0);

    Rasteron_Image* noiseImg = alloc_image("gradient_noise", size.height, size.width);

	// generating grid cells
    const unsigned xCellPoints = grid.xCells + 1; // includes leftmost and rightmost vertices +1
    const unsigned yCellPoints = grid.yCells + 1; // includes topmost and bottommost vertices +1

	Rasteron_Image* gridImg = alloc_image("grid", yCellPoints, xCellPoints);
	for (unsigned p = 0; p < gridImg->width * gridImg->height; p++) {
		double noiseVal = (double)rand() / (double)RAND_MAX; // random value between 0 and 1
		*(gridImg->data + p) = blendColors(grid.color1, grid.color2, noiseVal); // blending value between grid colors
	}

	// grid cell values
	unsigned* topLeft; unsigned* topRight; unsigned* botLeft; unsigned* botRight;
	const unsigned xSwitch = noiseImg->width / grid.xCells;
	const unsigned ySwitch = noiseImg->height / grid.yCells;

    for(unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        unsigned xOffset = p % noiseImg->width; // absolute X pixel offset
        unsigned yOffset = p / noiseImg->width; // absolute Y pixel offset
		
		// setting grid cell values
		if(xOffset == 0){ // repositions to correct row inside grid
			unsigned short row = (yOffset / ySwitch) * gridImg->width;

			topLeft = gridImg->data + row;
			topRight = gridImg->data + 1 + row;
			botLeft = gridImg->data + gridImg->width + row;
			botRight = gridImg->data + gridImg->width + 1 + row;
		}
		else if (xOffset % xSwitch == 0 && p > 0) { // increment to next column inside grid
			topLeft++; topRight++; botLeft++; botRight++;
		}

		// color determination
		double xFrac = (double)(xOffset % xSwitch) / (double)xSwitch; // relative X offset inside grid cell
		double yFrac = (double)(yOffset % ySwitch) / (double)ySwitch; // relative Y offset inside grid cell

		unsigned newColor = blendColors(
			blendColors(*topLeft, *topRight, xFrac),
			blendColors(*botLeft, *botRight, xFrac),
			yFrac
		);
        
        *(noiseImg->data + p) = newColor;
    }

	dealloc_image(gridImg);
    return noiseImg;
}

Rasteron_Image* noiseImgOp_warp(ImageSize size, const ColorGridTable* gridTable){
	assert(gridTable->gridCount > 0);

	Rasteron_Image** noiseImages = (Rasteron_Image**)malloc(sizeof(Rasteron_Image*) * gridTable->gridCount);
	
	for(unsigned l = 0; l < gridTable->gridCount; l++)
		*(noiseImages + l) = noiseImgOp_grid(size, gridTable->grids[l]);

	Rasteron_Image* fbmNoiseImg = copyImgOp(*noiseImages);
	for(unsigned l = 1; l < gridTable->gridCount; l++)
		for(unsigned p = 0; p < fbmNoiseImg->width * fbmNoiseImg->height; p++)
			*(fbmNoiseImg->data + p) = blendColors(*(fbmNoiseImg->data + p), *((*(noiseImages + l))->data + p), 0.5);

	for(unsigned l = 0; l < gridTable->gridCount; l++)
		dealloc_image(*(noiseImages + l));
	free(noiseImages);
}