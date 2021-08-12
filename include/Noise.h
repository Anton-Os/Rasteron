#include "Rasteron.h"
#include "Lattice.h"

#define MAX_NOISE_WEIGHTS 1048576 // 2 ^ 20

Rasteron_Image* createRandNoiseImg(uint32_t color1, uint32_t color2, const Rasteron_Image* ref); // creates interpolated white noise
// Rasteron_Lattice* createRandLattice(uint16_t width, uint16_t height);

#define MAX_GRADIENT_CELLS TWOPOWER(11) // 2046

typedef enum {
    NOISE_Octave,
    NOISE_Perlin
} NOISE_Type;

typedef struct {
	uint32_t startColor; // = TOTAL_BLACK_COLOR_CODE;
	uint32_t endColor; // = TOTAL_WHITE_COLOR_CODE;
	double xGradientVal; // = 0;
	double yGradientVal; // = 1; // default gradient will point up
} Rasteron_NoiseGradient;

typedef struct {
    Rasteron_NoiseGradient gradients[MAX_GRADIENT_CELLS];
	unsigned xCellDivs; // = 1; // x axis cell divisions
	unsigned yCellDivs; // = 1; // y axis cell divisions
} Rasteron_NoiseGradientLattice;


Rasteron_Image* createPerlinNoiseImg(
    const Rasteron_NoiseGradientLattice* lattice,
    const Rasteron_Image* refImage
);