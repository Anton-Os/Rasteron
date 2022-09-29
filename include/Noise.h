#ifndef RASTERON_NOISE_H

#include "Image.h"

#define MAX_NOISE_WEIGHTS TWOPOWER(20) // 1048576
#define MAX_GRADIENT_CELLS TWOPOWER(11) // 2046

typedef struct {
	unsigned xCellDivs; // x axis cell divisions
	unsigned yCellDivs; // y axis cell divisions
	unsigned color1;
	unsigned color2;
} GradientLattice;

Rasteron_Image* createNoiseImg_white(ImageSize size, uint32_t color1, uint32_t color2); // creates interpolated white noise
Rasteron_Image* createNoiseImg_gradient(ImageSize size, GradientLattice lattice);
// Rasteron_Image* createNoiseImg_octave(const Rasteron_Image* refImage, GradientLattice lattice1, GradientLattice lattice2);

#define RASTERON_NOISE_H
#endif