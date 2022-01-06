#include <time.h>

#include "Noise.h"

// Helper Operations

/* void addGradient(Rasteron_NoiseGradientTable* table, double x, double y){
    addColorGradient(
        table, x, y,
        TOTAL_BLACK_COLOR_CODE, TOTAL_WHITE_COLOR_CODE
    );
}

void addColorGradient(Rasteron_NoiseGradientTable* table, double x, double y, uint32_t color1, uint32_t color2){
    Rasteron_NoiseGradient gradient;
    gradient.color1 = color1;
    gradient.color2 = color2;
    gradient.x = x;
    gradient.y = y;

    // table->gradients[table->gradientsCount] = gradient;
    // table->gradientsCount++;
} */

// Noise Image Operations

Rasteron_Image* createWhiteNoiseImg(uint32_t color1, uint32_t color2, const Rasteron_Image* refImage){
    if (refImage == NULL) {
		puts("Cannot create white noise! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* randNoiseImg = allocNewImg("white-noise", refImage->width, refImage->height);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < randNoiseImg->width * randNoiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		uint32_t color = itrpolate(color1, color2, noiseVal);
		*(randNoiseImg->data + p) = color; // see if this works
    }

    return randNoiseImg;
}

Rasteron_Image* createGradientNoiseImg( const Rasteron_NoiseGradientTable* lattice, const Rasteron_Image* refImage){
    if(lattice == NULL || refImage == NULL){
        puts("Cannot create gradient noise! Null pointers provided as inputs!");
        return NULL;
    }

    Rasteron_Image* gradientNoiseImg = allocNewImg("gradient-noise", refImage->width, refImage->height);

    const unsigned xCellPoints = lattice->xCellDivs + 1;
    const unsigned yCellPoints = lattice->yCellDivs + 1;
    const unsigned xSwitch = gradientNoiseImg->width / lattice->xCellDivs;
    const unsigned ySwitch = gradientNoiseImg->height / lattice->yCellDivs;

    // unsigned noiseIndex_topLeft = 0;
    // unsigned noiseIndex_topRight = 1;
    // unsigned noiseIndex_botLeft = xCellPoints;
    // unsigned noiseIndex_botRight = xCellPoints + 1;

    unsigned color = 0xFFFF0000; // for testing
    for(unsigned p = 0; p < gradientNoiseImg->width * gradientNoiseImg->height; p++){
        unsigned xOffset = p % gradientNoiseImg->width;
        unsigned yOffset = p / gradientNoiseImg->width;

		if (yOffset % ySwitch == 0 && xOffset == 0) // requires the grid shift far left and down 1
			color = 0xFF00FF00; // for testing
		
		else if (xOffset % xSwitch == 0) // requires the grid shift right 1
			color = (color == 0xFF0000FF)? 0xFFFF0000 : 0xFF0000FF; // for testing
        
        *(gradientNoiseImg->data + p) = color;
    }

    return gradientNoiseImg;
}