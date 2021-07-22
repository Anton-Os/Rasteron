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

    return randNoiseImg;
}