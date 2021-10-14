#include "Pattern.h"

void addPatternColor(Rasteron_Pattern* pattern, Rasteron_PatternColor patColor){
    // Implement body
}

uint32_t lookupColor(Rasteron_Pattern* pattern, uint8_t val){
    // Implement Body
}

Rasteron_Image* genPatternImage(const Rasteron_Image* refImage, const Rasteron_Pattern* pattern){
    if (refImage == NULL) {
		puts("Cannot create filter image! Null pointer provided as reference image!");
		return NULL;
	}
    
    Rasteron_Image* patternImage = allocNewImg("pattern", refImage->width, refImage->height);

    // Implement Pattern Logic

    return patternImage;
}