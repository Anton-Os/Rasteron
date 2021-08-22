#include <time.h>

#include "Noise.h"

// Helper Operations

void addGradient(Rasteron_NoiseGradientTable* noiseGradientTable, double x, double y){
    addColorGradient(
        noiseGradientTable, x, y,
        TOTAL_BLACK_COLOR_CODE, TOTAL_WHITE_COLOR_CODE
    );
}

void addColorGradient(Rasteron_NoiseGradientTable* noiseGradientTable, double x, double y, uint32_t startColor, uint32_t endColor){
    Rasteron_NoiseGradient gradient;
    gradient.startColor = startColor;
    gradient.endColor = endColor;
    gradient.x = x;
    gradient.y = y;

    noiseGradientTable->gradients[noiseGradientTable->gradientsCount] = gradient;
    noiseGradientTable->gradientsCount++;
}

// Noise Image Operations

Rasteron_Image* createRandNoiseImg(uint32_t color1, uint32_t color2, const Rasteron_Image* refImage){
    if (refImage == NULL) {
		puts("Cannot create random noise! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* randNoiseImg = allocNewImg("random-noise", refImage->width, refImage->height);

    // Noise Generation Logic
    double noiseVal;
    for (unsigned p = 0; p < randNoiseImg->width * randNoiseImg->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
		uint32_t color = interpolateColor(color1, color2, noiseVal);
		*(randNoiseImg->data + p) = color; // see if this works
    }

    return randNoiseImg;
}

Rasteron_Image* createPerlinNoiseImg( const Rasteron_NoiseGradientTable* lattice, const Rasteron_Image* refImage){
    if(lattice == NULL || refImage == NULL){
        puts("Cannot create lattice noise! Null pointers provided as inputs!");
        return NULL;
    }

    Rasteron_Image* perlinNoiseImg = allocNewImg("perlin-noise", refImage->width, refImage->height);

    const unsigned xCellPoints = lattice->xCellDivs + 1;
    const unsigned yCellPoints = lattice->yCellDivs + 1;
    const unsigned xSwitchOffset = perlinNoiseImg->width / lattice->xCellDivs;
    const unsigned ySwitchOffset = perlinNoiseImg->height / lattice->yCellDivs;

    unsigned noiseIndex_topLeft = 0;
    unsigned noiseIndex_topRight = 1;
    unsigned noiseIndex_botLeft = xCellPoints;
    unsigned noiseIndex_botRight = xCellPoints + 1;
    Rasteron_NoiseGradient noiseVal_topLeft = lattice->gradients[noiseIndex_topLeft];
    Rasteron_NoiseGradient noiseVal_topRight = lattice->gradients[noiseIndex_topRight];
    Rasteron_NoiseGradient noiseVal_botLeft = lattice->gradients[noiseIndex_botLeft];
    Rasteron_NoiseGradient noiseVal_botRight = lattice->gradients[noiseIndex_botRight];

    unsigned outColor = 0xFFFF0000; // for testing
    for(unsigned p = 0; p < perlinNoiseImg->width * perlinNoiseImg->height; p++){
        unsigned xOffset = p % perlinNoiseImg->width;
        unsigned yOffset = p / perlinNoiseImg->width;

		if (yOffset % ySwitchOffset == 0 && xOffset == 0) { // requires the grid shift far left and down 1
			outColor = 0xFF00FF00; // for testing
		}
		else if (xOffset % xSwitchOffset == 0) { // requires the grid shift right 1
			outColor = (outColor == 0xFF0000FF)? 0xFFFF0000 : 0xFF0000FF; // for testing
		}
        
        *(perlinNoiseImg->data + p) = outColor;
    }

    return perlinNoiseImg;
}