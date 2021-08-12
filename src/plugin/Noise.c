#include <time.h>

#include "Noise.h"

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

Rasteron_Image* createPerlinNoiseImg( const Rasteron_NoiseGradientLattice* lattice, const Rasteron_Image* refImage){
    if(lattice == NULL || refImage == NULL){
        puts("Cannot create lattice noise! Null pointers provided as inputs!");
        return NULL;
    }

    Rasteron_Image* perlinNoiseImg = allocNewImg("perlin-noise", refImage->width, refImage->height);

    const unsigned xCellPoints = lattice->xCellDivs + 1;
    const unsigned yCellPoints = lattice->yCellDivs + 1;
    const unsigned xSwitchOffset = perlinNoiseImg->width / lattice->xCellDivs;
    const unsigned ySwitchOffset = perlinNoiseImg->height / lattice->yCellDivs;
    Rasteron_NoiseGradient noiseVal_topLeft = lattice->gradients[0];
    Rasteron_NoiseGradient noiseVal_topRight = lattice->gradients[1];
    Rasteron_NoiseGradient noiseVal_botLeft = lattice->gradients[xCellPoints];
    Rasteron_NoiseGradient noiseVal_botRight = lattice->gradients[xCellPoints + 1];

    /* for(unsigned cellCount = 0; cellCount < xCellPoints * yCellPoints; cellCount++){
        noiseVal_topLeft = lattice->gradients[cellCount];
        noiseVal_topRight = lattice->gradients[cellCount + 1];
        noiseVal_botLeft = lattice->gradients[cellCount + xCellPoints];
        noiseVal_topRight = lattice->gradients[cellCount + xCellPoints + 1];

        // iterate through cooresponding chunk of the new image!
    } */

    unsigned outColor = 0xFF00000;
    for(unsigned p = 0; p < perlinNoiseImg->width * perlinNoiseImg->height; p++){
        unsigned xOffset = p % perlinNoiseImg->width;
        unsigned yOffset = p / perlinNoiseImg->width;

        // calculate updated gradients for current points

        if(xOffset % xSwitchOffset == 0) outColor = 0xFF00FF00; // for testing
        if(yOffset % ySwitchOffset == 0) outColor = 0xFF0000FF; // for testing
        // assign pixel value based on gradient
    }

    return perlinNoiseImg;
}