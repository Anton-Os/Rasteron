#include <time.h>

#include "Noise.h"

// Noise Image Operations

Rasteron_Image* createWhiteNoiseImg(const Rasteron_Image* refImage, uint32_t color1, uint32_t color2){
    if (refImage == NULL) {
		puts("Cannot create white noise! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* whiteNoiseImg = allocNewImg("white-noise", refImage->height, refImage->width);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < whiteNoiseImg->width * whiteNoiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		*(whiteNoiseImg->data + p) = itrpolate(color1, color2, noiseVal);
    }

    return whiteNoiseImg;
}

Rasteron_Image* createGradientNoiseImg(const Rasteron_Image* refImage, const Rasteron_GradientNoise* noise){
    if(noise == NULL || refImage == NULL){
        puts("Cannot create gradient noise! Null pointers provided as inputs!");
        return NULL;
    }

    Rasteron_Image* gradientNoiseImg = allocNewImg("gradient-noise", refImage->height, refImage->width);

    const unsigned xCellPoints = noise->xCellDivs + 1; // includes leftmost and rightmost vertices +1
    const unsigned yCellPoints = noise->yCellDivs + 1; // includes topmost and bottommost vertices +1

	Rasteron_Image* latticeCells = allocNewImg("cell-points", yCellPoints, xCellPoints);
	double noiseVal;
	for (unsigned p = 0; p < latticeCells->width * latticeCells->height; p++) {
		noiseVal = (double)rand() / (double)RAND_MAX;
		*(latticeCells->data + p) = itrpolate(noise->color1, noise->color2, noiseVal); // use this instead
		// switch (p % 4) {
		// case 0: *(latticeCells->data + p) = 0xFFFFFFFF; /* 0x00000001; */ break;
		// case 1: *(latticeCells->data + p) = 0xFF00FFFF; /* 0x00000002; */ break;
		// case 2: *(latticeCells->data + p) = 0xFFFF00FF; /* 0x00000003; */ break;
		// case 3: *(latticeCells->data + p) = 0xFFFFFF00; /* 0x00000004; */ break;
		// }
	}

	// tracks position of the current gradient cell
	unsigned* topLeft; unsigned* topRight; unsigned* botLeft; unsigned* botRight;

	unsigned color = 0xFFFFFFFF;
	const unsigned xSwitch = gradientNoiseImg->width / noise->xCellDivs;
	const unsigned ySwitch = gradientNoiseImg->height / noise->yCellDivs;

    for(unsigned p = 0; p < gradientNoiseImg->width * gradientNoiseImg->height; p++){
        unsigned xOffset = p % gradientNoiseImg->width; // absolute offset along width
        unsigned yOffset = p / gradientNoiseImg->width; // absolute offset along height
		double xFrac = (double)(xOffset % xSwitch) / (double)xSwitch; // fractional offset relative to current cell along width
		double yFrac = (double)(yOffset % ySwitch) / (double)ySwitch; // fractional offset relative to current cell along height

		if(xOffset == 0){
			unsigned short yInc = (yOffset / ySwitch) * latticeCells->width;

			// resets to the beginning of the correct row
			topLeft = latticeCells->data + yInc;
			topRight = latticeCells->data + 1 + yInc;
			botLeft = latticeCells->data + latticeCells->width + yInc;
			botRight = latticeCells->data + latticeCells->width + 1 + yInc;
		}
		else if (xOffset % xSwitch == 0 && p > 0) { // increment to next gradient cell
			topLeft++; topRight++; botLeft++; botRight++;
		}

		/* color = itrpolate(
			itrpolate(*topLeft, *topRight, xFrac), 
			itrpolate(*botLeft, *botRight, xFrac), 
			yFrac
		); */
		color = *topLeft; // for testing
		color = itrpolate(*topLeft, *botLeft, xFrac);
        
        *(gradientNoiseImg->data + p) = color;
    }

	deleteImg(latticeCells);
    return gradientNoiseImg;
}