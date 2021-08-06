#include "Rasteron.h"
#include "Lattice.h"

#define MAX_NOISE_WEIGHTS 1048576 // 2 ^ 20

Rasteron_Image* createRandNoiseImg(uint32_t color1, uint32_t color2, const Rasteron_Image* ref);
Rasteron_Lattice* createRandLattice(uint16_t width, uint16_t height);

typedef enum {
    NOISE_Octave,
    NOISE_Perlin
} NOISE_Type;

/* typedef struct {
    NOISE_Type type;
    unsigned latticeWidth;
    unsigned latticeHeight;
    double* latticeData;
} Rasteron_NoiseLattice; */

Rasteron_Image* createLatticeNoiseImg(
    uint32_t color1,
    uint32_t color2,
    NOISE_Type noiseType,
    const Rasteron_Lattice* lattice, 
    const Rasteron_Image* refImage
);