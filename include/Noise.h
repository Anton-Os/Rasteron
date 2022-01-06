#include "Rasteron.h"
#include "Heightmap.h"

#define MAX_NOISE_WEIGHTS TWOPOWER(20) // 1048576
#define MAX_GRADIENT_CELLS TWOPOWER(11) // 2046

typedef struct {
	unsigned xCellDivs; // = 1; // x axis cell divisions
	unsigned yCellDivs; // = 1; // y axis cell divisions
	unsigned color1;
	unsigned color2;
} Rasteron_NoiseGradientTable;

// void addGradient(Rasteron_NoiseGradientTable* table, double x, double y);
// void addColorGradient(Rasteron_NoiseGradientTable* table, double x, double y, uint32_t color1, uint32_t color2);

Rasteron_Image* createWhiteNoiseImg(uint32_t color1, uint32_t color2, const Rasteron_Image* ref); // creates interpolated white noise
Rasteron_Image* createGradientNoiseImg(
    const Rasteron_NoiseGradientTable* lattice,
    const Rasteron_Image* refImage
);