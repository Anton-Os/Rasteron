/* Inspired by Stephen Wolfram, I will generate 1D patterns line by line */

#include "Rasteron.h"

#define NOT_PATTERN_LOOKUP 255 // max value of type uint8_t
#define MAX_PATTERN_COLORS TWOPOWER(11) // 2046

typedef struct {
    uint32_t color;
    uint8_t state; // state is used as a lookup for color
} Rasteron_PatternColor;

typedef struct {
    uint32_t defaultColor; // utilized if val lookup is unsuccessful
    uint8_t (*ruleCallback)(uint8_t, uint8_t); // rule determining new value based on left and right neighbor

    Rasteron_PatternColor patternColors[MAX_PATTERN_COLORS];
    unsigned patternCount;
} Rasteron_Pattern;

uint32_t lookupPatternColor(Rasteron_Pattern* pattern, uint8_t state);
uint8_t lookupPatternState(Rasteron_Pattern* pattern, uint32_t color);
void addPatternColor(Rasteron_Pattern* pattern, uint32_t color, uint8_t state);
Rasteron_Image* createImgPattern(const Rasteron_Image* refImage, const Rasteron_Pattern* pattern);