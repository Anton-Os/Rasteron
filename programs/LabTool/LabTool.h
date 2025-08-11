#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"

extern ColorSwatch _swatch; // = { 0xFF888888, { 0xFFFF8888, 0xFF88FF88, 0xFF8888FF, 0xFF333333, 0xFFEEEEEE, 0xFF888800, 0xFF880088, 0xFF008888 }, 0xF }; // determines color oprations

// Rules

unsigned addlineRules(unsigned color, unsigned neighbors[2]);
unsigned serpinskyRules(unsigned color, unsigned neighbors[2]);

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

// Experimental Images

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop); // Flipping, mirroring, and cropping
Rasteron_Image* nestboxesImgOp(double x, double y); // Looped nesting of ever-smaller squares
Rasteron_Image* lensesImgOp(int channel); // lenses image to color ranges and performing channel operations
Rasteron_Image* typographyImgOp(unsigned color1, unsigned color2); // Drawing text example
Rasteron_Image* fisheyeImgOp(unsigned short points); // Vornoi and other distance related functions
Rasteron_Image* mozaicImgOp(double z1, double z2); // Creates a regular repeating triangular pattern
Rasteron_Image* interferenceImgOp(unsigned short inc, unsigned short dec); // Combines grid tiles, feild operations and seeding to create the result
Rasteron_Image* perturbImgOp(double xCenter, double yCenter); // Purturbs a cross image using red as x, green as y, and blue for lightness
Rasteron_Image* ballingImgOp(double size); // Black and white blobs transformed into gradients effects
Rasteron_Image* stratifyImgOp(unsigned short levels); // Stratifies colors into discrete intervals based on brightness
Rasteron_Image* barkodeImgOp(unsigned short iters, unsigned color1, unsigned color2);
Rasteron_Image* chaosImgOp(unsigned short rows, unsigned short cols);
Rasteron_Image* euclidTileImgOp(int mode, unsigned short density, double xFactor, double yFactor);
Rasteron_Image* nuTileImgOp(int mode, unsigned short density, double xFactor, double yFactor);
Rasteron_Image* graterImgOp(unsigned color1, unsigned color2);
Rasteron_Image* mildewImgOp(unsigned short iters);
Rasteron_Image* fuzzlikeImgOp(unsigned short iters);
Rasteron_Image* oozelikeImgOp(unsigned short colorMode);
Rasteron_Image* recurrantImgOp(unsigned short iters);
Rasteron_Image* intersectImgOp(double angle1, double angle2);
Rasteron_Image* displacerImgOp(unsigned short cells, unsigned color1, unsigned color2);
Rasteron_Image* bilineImgOp(unsigned color, unsigned short variant);
Rasteron_Image* arcaneImgOp(double radius, unsigned short count);
Rasteron_Image* hourglassesImgOp(unsigned color1, unsigned color2);
Rasteron_Image* ditherImgOp(ref_image_t targetImg, uint8_t thresh, unsigned color1, unsigned color2);
Rasteron_Image* turbulentImgOp(ref_image_t targetImg, uint8_t count, mixCallback callback);
Rasteron_Image* raycastImgOp(float* points, unsigned pointCount, double distance);
Rasteron_Image* subtImgOp(nebrCallback8 callback, unsigned colors[4]);
Rasteron_Image* heightImgOp(Rasteron_Heightmap* heightmap, unsigned color1, unsigned color2);
Rasteron_Image* remixImgOp(Rasteron_Image* image1, Rasteron_Image* image2);