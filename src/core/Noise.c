#include <time.h>

#include "Rasteron.h"

// Noise Image Operations

Rasteron_Image* noiseImgOp_white(ImageSize size, uint32_t color1, uint32_t color2){
    Rasteron_Image* noiseImg = RASTERON_ALLOC("white_noise", size.height, size.width);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < noiseImg->width * noiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		*(noiseImg->data + p) = colors_fuse(color1, color2, noiseVal);
    }

    return noiseImg;
}

Rasteron_Image* noiseImgOp_grid(ImageSize size, ColorGrid grid){
	assert(grid.xCells > 0 && grid.yCells > 0);

    Rasteron_Image* noiseImg = RASTERON_ALLOC("gradient_noise", size.height, size.width);

	// generating grid cells
    const unsigned xCellPoints = grid.xCells + 1; // includes leftmost and rightmost vertices +1
    const unsigned yCellPoints = grid.yCells + 1; // includes topmost and bottommost vertices +1

	Rasteron_Image* gridImg = RASTERON_ALLOC("grid", yCellPoints, xCellPoints);
	for (unsigned p = 0; p < gridImg->width * gridImg->height; p++) {
		double noiseVal = (double)rand() / (double)RAND_MAX; // random value between 0 and 1
		*(gridImg->data + p) = colors_blend(grid.color1, grid.color2, noiseVal); // blending value between grid colors
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

		unsigned newColor = colors_blend(
			colors_blend(*topLeft, *topRight, xFrac),
			colors_blend(*botLeft, *botRight, xFrac),
			yFrac
		);
        
        *(noiseImg->data + p) = newColor;
    }

	RASTERON_DEALLOC(gridImg);
    return noiseImg;
}

Rasteron_Image* noiseImgOp_octave(ImageSize size, ColorGrid grid, unsigned short octaves){
	assert(grid.xCells > 0 && grid.yCells > 0);

	Rasteron_Image* noiseImg = noiseImgOp_grid(size, grid);

	if(octaves > 1)
		for(unsigned o = 0; o < octaves; o++){
			Rasteron_Image* octaveImg = resizeImgOp((ImageSize){ noiseImg->height / pow(2, o + 1), noiseImg->width / pow(2, o + 1)}, noiseImg);
			for(unsigned p = 0; p < noiseImg->width * noiseImg->width; p++){
				unsigned xOffset = (p % noiseImg->width) % octaveImg->width;
				unsigned yOffset = (p / noiseImg->width) % octaveImg->height;
				*(noiseImg->data + p) = colors_blend(*(noiseImg->data + p), *(octaveImg->data + (yOffset * octaveImg->width) + xOffset), 0.5); // blend factor is important
			}
			RASTERON_DEALLOC(octaveImg);
		}

	return noiseImg; // TODO: Return fluxNoise image
}

/* Rasteron_Image* noiseImgOp_pink(ImageSize size, ColorGrid grid){
	assert(grid.xCells > 0 && grid.yCells > 0);

	Rasteron_Image* noiseImg = noiseImgOp_grid(size, grid);

	double* levelPoints = (double*)malloc(sizeof(double) * (grid.xCells + 1) * (grid.yCells + 1));

	for(unsigned l = 0; l < (grid.xCells + 1) * (grid.yCells + 1); l++)
		*(levelPoints + l) = rand() / (double)RAND_MAX;

	for(unsigned p = 0; p < size.height * size.width; p++){
		// TODO: Use level points to interpolate colors

		double colorLevel = grayify8(*(noiseImg->data + p)) / 256.0;
		colorLevel += ((rand() / (double)RAND_MAX) / 5.0) - 0.1; // adjust small amount up or down

		*(noiseImg->data + p) = color_level(*(noiseImg->data + p), colorLevel);
	}

	free(levelPoints);

	return noiseImg; // TODO: Return fluxNoise image
} */