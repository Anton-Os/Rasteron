#include "Rasteron.h"
#include "Heightmap.h"

#define MAX_NOISE_WEIGHTS TWOPOWER(20) // 1048576
#define MAX_GRADIENT_CELLS TWOPOWER(11) // 2046

typedef enum {
    NOISE_Octave,
    NOISE_Perlin
} NOISE_Type;

typedef struct {
	double x; // x direction pointed by gradient
	double y; // y direction pointed by gradient
	uint32_t color1; // color at the start of gradient
	uint32_t color2; // color at the end of graident
} Rasteron_NoiseGradient;

typedef struct {
	unsigned xCellDivs; // = 1; // x axis cell divisions
	unsigned yCellDivs; // = 1; // y axis cell divisions

	unsigned gradientsCount;
	Rasteron_NoiseGradient gradients[MAX_GRADIENT_CELLS];
} Rasteron_NoiseGradientTable;

void addGradient(Rasteron_NoiseGradientTable* table, double x, double y);
void addColorGradient(Rasteron_NoiseGradientTable* table, double x, double y, uint32_t color1, uint32_t color2);

Rasteron_Image* createWhiteNoiseImg(uint32_t color1, uint32_t color2, const Rasteron_Image* ref); // creates interpolated white noise

Rasteron_Image* createPerlinNoiseImg(
    const Rasteron_NoiseGradientTable* lattice,
    const Rasteron_Image* refImage
);