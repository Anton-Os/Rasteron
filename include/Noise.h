#include "Rasteron.h";

#define MAX_NOISE_WEIGHTS 1048576 // 2 ^ 20

void seedRandGen(); // seeds the random number generator
Rasteron_Image* createRandomNoiseImg(uin32_t color1, uint32_t color2, const Rasteron_Image* ref);