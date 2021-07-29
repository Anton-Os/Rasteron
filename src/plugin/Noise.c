#include <time.h>

#include "Noise.h"

void seedRandGen(){
    srand (time(NULL));
}

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

Rasteron_Lattice* createRandLattice(uint16_t width, uint16_t height){
    Rasteron_Lattice* lattice = allocNewLattice((uint32_t)width, (uint32_t)height, 0.0, 1.0);

    for(unsigned p = 0; p < lattice->width * lattice->height; p++)
        *(lattice->data + p) = (double)rand() / (double)RAND_MAX; // random values are generated

    return lattice;
}

Rasteron_Image* createLatticeNoiseImg(uint32_t color1, uint32_t color2, NOISE_Type noiseType, const Rasteron_Lattice* lattice, const Rasteron_Image* refImage){
    if(lattice == NULL || refImage == NULL){
        puts("Cannot create lattice noise! Null pointers provided as inputs!");
        return NULL;
    }

    Rasteron_Image* latticeNoiseImg = allocNewImg("lattice-noise", refImage->width, refImage->height);
}