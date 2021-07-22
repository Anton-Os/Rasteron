#include "Noise.h"

Rasteron_Image* createRandomNoiseImg(uin32_t color1, uint32_t color2, const Rasteron_Image* refImage){
    if (ref == NULL) {
		puts("Cannot create random noise! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* randNoiseImg = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
    randNoise->name = "random-noise";

    randNoise->height = refImage->height;
    randNoise->width = refImage->width;
    randNoise->data data = (uint32_t*)malloc(randNoise->width * randNoise->height * sizeof(uint32_t));

    // Noise Generation Logic
    /* uint8_t loRedBit = ((color1 & RED_BITS_MASK) < (color2 & RED_BITS_MASK)) ? color1 & RED_BITS_MASK : color2 & RED_BITS_MASK;
    uint8_t loGreenBit = ((color1 & GREEN_BITS_MASK) < (color2 & GREEN_BITS_MASK)) ? color1 & GREEN_BITS_MASK : color2 & GREEN_BITS_MASK;
    uint8_t loBlueBit = ((color1 & BLUE_BITS_MASK) < (color2 & BLUE_BITS_MASK)) ? color1 & BLUE_BITS_MASK : color2 & BLUE_BITS_MASK;
    uint8_t hiRedBit = ((color1 & RED_BITS_MASK) > (color2 & RED_BITS_MASK)) ? color1 & RED_BITS_MASK : color2 & RED_BITS_MASK;
    uint8_t hiGreenBit = ((color1 & GREEN_BITS_MASK) > (color2 & GREEN_BITS_MASK)) ? color1 & GREEN_BITS_MASK : color2 & GREEN_BITS_MASK;
    uint8_t hiBlueBit = ((color1 & BLUE_BITS_MASK) > (color2 & BLUE_BITS_MASK)) ? color1 & BLUE_BITS_MASK : color2 & BLUE_BITS_MASK; */
    double noiseVal;
    for (unsigned p = 0; p < heightmap->width * heightmap->height; p++){
        noiseVal = (double)rand() / (double)RAND_MAX;
        // TODO: Continue processing logic, use Heighmap generation as reference
    }

    return randNoiseImg;
}