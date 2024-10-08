#ifndef RASTERON_EXPERIMENTAL_H

#define RASTERON_ENABLE_ANIM
#define RASTERON_ENABLE_FONT

#include "Rasteron.h"

// TODO: Incldue global definitions

Rasteron_Image* oragamiImgOp(enum FLIP_Type flip, double xCrop, double yCrop); // Flipping, mirroring, and cropping
Rasteron_Image* nestboxesImgOp(double x, double y); // Looped nesting of ever-smaller squares
Rasteron_Image* lensesImgOp(enum CHANNEL_Type channel); // lenses image to color ranges and performing channel operations
Rasteron_Image* hypnosisImgOp(unsigned pArg, unsigned color1, unsigned color2); // Visual lense effects and enhancements to base image
Rasteron_Image* typographyImgOp(unsigned color1, unsigned color2); // Drawing text example
Rasteron_Image* fisheyeImgOp(unsigned short points); // Vornoi and other distance related functions
Rasteron_Image* mozaicImgOp(double z1, double z2); // Creates a regular repeating triangular pattern
Rasteron_Image* interferenceImgOp(unsigned short inc, unsigned short dec); // Combines grid tiles, feild operations and seeding to create the result
Rasteron_Image* perturbImgOp(double xCenter, double yCenter); // Purturbs a cross image using red as x, green as y, and blue for lightness
Rasteron_Image* ballingImgOp(double size); // Black and white blobs transformed into gradients effects
Rasteron_Image* stratifyImgOp(unsigned short levels); // Stratifies colors into discrete intervals based on brightness
Rasteron_Image* barkodeImgOp(unsigned short iters, unsigned color1, unsigned color2);
Rasteron_Image* combImgOp(unsigned short rows, unsigned short cols);
Rasteron_Image* truschetImgOp(unsigned short wDiv, unsigned short hDiv);
Rasteron_Image* euclidTileImgOp(int mode, unsigned short density, double xFactor, double yFactor);
Rasteron_Image* nuTileImgOp(int mode, unsigned short density, double xFactor, double yFactor);
Rasteron_Image* graterImgOp(unsigned color1, unsigned color2);
Rasteron_Image* mildewImgOp(unsigned short iters);
Rasteron_Image* oozelikeImgOp(unsigned short colorMode);
Rasteron_Image* recurrantImgOp(unsigned short iters);
Rasteron_Image* intersectImgOp(double angle1, double angle2);
Rasteron_Image* swirlyImgOp(double swirlFactor);
Rasteron_Image* displacerImgOp(unsigned short cells, unsigned color1, unsigned color2);
Rasteron_Image* bilineImgOp(unsigned color, unsigned short variant);
Rasteron_Image* arcaneImgOp(double radius, unsigned short count);
Rasteron_Image* ultImgOp(short seed, unsigned short factor, double x1, double x2, double y1, double y2);

Rasteron_Image* expImgOp1(); // placeholder for new images

#define RASTERON_EXPERIMENTAL_H
#endif