#ifndef RASTERON_TOOLBOX_H

#include "support.h"

#include <math.h>
#include <string.h>

void seedRandGen(); // seeds the random number generator
void replaceFwdSlash(char* str); // utility for replacing forward-slashes with back-slashes

void makeColor(uint32_t* raster, unsigned pixels, uint32_t colorVal); // makes image data into uniform color
void changeColor(uint32_t* raster, unsigned pixels, uint32_t newColor, uint32_t oldColor); // switches oldColor to newColor in image
void switchRB(uint32_t* raster, unsigned pixels); // switch red and blue bits
void switchGB(uint32_t* raster, unsigned pixels); // switch green and blue bits
void switchRG(uint32_t* raster, unsigned pixels); // switch red and green bits
uint32_t genRandColorVal();
// enum CHANNEL_Type getDominantChan(unsigned color);
uint8_t getLoChanBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t getHiChanBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
int8_t getLightDiff(uint32_t color1, uint32_t color2); // returns positive if lighter, negative if darker, zero on match
int8_t getChanDiff(uint32_t color1, uint32_t color2, CHANNEL_Type channel); // returns positive if lighter, negative if darker, zero on match
double getPixDist(unsigned p1, unsigned p2, unsigned imageWidth); // gets pixel distance based on indices

uint8_t grayify8(uint32_t refColor); // produces a 8 bit grey value based on reference color
uint32_t grayify32(uint32_t refColor); // produces a 32 bit grey value based on reference color
uint8_t fract8(uint8_t refColor, double frac); // produces an 8 bit color value based on fraction of reference color
uint32_t fract32(uint32_t refColor, double frac); // produces an 32 bit color value based on fraction of reference color
uint32_t blend(uint32_t color1, uint32_t color2, double bVal); // blends values between two colors
uint32_t fuse(uint32_t color1, uint32_t color2, double iVal); // fuse between hi and low colors

#define RASTERON_TOOLBOX_H
#endif