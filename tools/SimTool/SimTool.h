
#define NSIM_GROW 0.1
#define NSIM_COUNT 12

#define SWATCH_L _swatch.colors[SWATCH_Light] // from Tool.h, but used here for ease of use
#define SWATCH_RA _swatch.colors[SWATCH_Red_Add] // from Tool.h, but used here for ease of use
#define SWATCH_RS _swatch.colors[SWATCH_Red_Sub] // from Tool.h, but used here for ease of use
#define SWATCH_GA _swatch.colors[SWATCH_Green_Add] // from Tool.h, but used here for ease of use
#define SWATCH_GS _swatch.colors[SWATCH_Green_Sub] // from Tool.h, but used here for ease of use
#define SWATCH_BA _swatch.colors[SWATCH_Blue_Add] // from Tool.h, but used here for ease of use
#define SWATCH_BS _swatch.colors[SWATCH_Blue_Sub] // from Tool.h, but used here for ease of use
#define SWATCH_D _swatch.colors[SWATCH_Dark] // from Tool.h, but used here for ease of use

#include "Rasteron.h"

extern ColorSwatch _swatch;

// Scanline Rules

unsigned addlineRules(unsigned color, unsigned neighbors[2]);
unsigned levelineRules(unsigned color, unsigned neighbors[2]);
unsigned serpinskyRules(unsigned color, unsigned neighbors[2]);

// Full-Cell Rules

unsigned shuffleRules(unsigned color, unsigned neighbors[8]);
unsigned conwayRules(unsigned color, unsigned neighbors[8]);
unsigned randWalkRules(unsigned color, unsigned neighbors[8]);
unsigned amplifyRules(unsigned color, unsigned neighbors[8]);
unsigned recursiveRules(unsigned color, unsigned neighbors[8]);
unsigned bloomRules(unsigned color, unsigned neighbors[8]);
unsigned unbloomRules(unsigned color, unsigned neighbors[8]);
unsigned levelRules(unsigned color, unsigned neighbors[8]);
unsigned matchRules(unsigned color, unsigned neighbors[8]);
unsigned colorizeRules(unsigned color, unsigned neighbors[8]);
unsigned scatterRules(unsigned color, unsigned neighbors[8]);
unsigned flipRules(unsigned color, unsigned neighbors[8]);
unsigned wolframRules(unsigned color, unsigned neighbors[8]);

// Image Operations

Rasteron_Image* seedImgOp(Rasteron_Image* refImg, double prob);
Rasteron_Image* growImgOp(Rasteron_Image* refImg, double balance, double exFactor);
Rasteron_Image* feedImgOp(ref_image_t refImg, unsigned short iters);
Rasteron_Image* simulationImgOp(ref_image_t refImage, unsigned short iters, nebrCallback8 callback);