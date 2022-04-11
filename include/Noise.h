#ifndef RASTERON_NOISE_H

#include "Image.h"
// #include "Cellwise.h"

#define MAX_NOISE_WEIGHTS TWOPOWER(20) // 1048576
#define MAX_GRADIENT_CELLS TWOPOWER(11) // 2046

typedef struct {
	unsigned xCellDivs; // x axis cell divisions
	unsigned yCellDivs; // y axis cell divisions
	unsigned color1;
	unsigned color2;
} Rasteron_GradientNoise;

Rasteron_Image* createWhiteNoiseImg(const Rasteron_Image* ref, uint32_t color1, uint32_t color2); // creates interpolated white noise
Rasteron_Image* createGradientNoiseImg( // perlin-like noise function
	const Rasteron_Image* refImage,
    const Rasteron_GradientNoise* lattice
);
/* Rasteron_Image* createFlowFieldImg( // creates flow field based on noise function
	const Rasteron_Image* refImage,
    const Rasteron_GradientNoise* lattice
) */

#define RASTERON_NOISE_H
#endif