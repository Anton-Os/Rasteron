#include "Pattern.h"

uint32_t lookupPatternColor(Rasteron_Pattern* pattern, uint8_t state){
    for(unsigned c = 0; c < pattern->patternCount; c++)
        if(pattern->patternColors[c].state == state) return pattern->patternColors[c].color;

    return pattern->defaultColor; // no matching state found
}

uint8_t lookupPatternState(Rasteron_Pattern* pattern, uint32_t color){
    for(unsigned c = 0; c < pattern->patternCount; c++)
        if(pattern->patternColors[c].color == color) return pattern->patternColors[c].state;

    return 0; // no matching state found
}

void addPatternColor(Rasteron_Pattern* pattern, uint32_t color, uint8_t state){
    pattern->patternColors[pattern->patternCount].color = color;
    pattern->patternColors[pattern->patternCount].state = state;
    pattern->patternCount++;
}

Rasteron_Image* createImgPattern(const Rasteron_Image* refImage, const Rasteron_Pattern* pattern){
    if (refImage == NULL) {
		puts("Cannot create pattern image! Null pointer provided as reference image!");
		return NULL;
	}
    
    Rasteron_Image* patternImage = allocNewImg("pattern", refImage->width, refImage->height);

    unsigned leftState, rightState, currentState;
    const unsigned startPixelIndex = 0; const unsigned endPixelIndex = (refImage->width * refImage->height) - 1;
    for(unsigned p = 0; p < patternImage; p++){
        if(p % patternImage->width == 0 || p == startPixelIndex) leftState = 0;
        else leftState = lookupPatternState(*(patternImage->data + p - 1));

        if((p % patternImage->width) - 1 == 0 || p == endPixelIndex) rightState = 0;
        else rightState = lookupPatternState(*(patternImage->data + p + 1));
        
        currentState = pattern->ruleCallback(leftState, rightState);
        *(perlinNoiseImg->data + p) = lookupPatternColor(currentState);
    }

    return patternImage;
}