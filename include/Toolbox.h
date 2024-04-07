#ifndef RASTERON_TOOLBOX_H

#include "support_def.h"
#include "typedef.h"

#define RAND_COLOR() (uint32_t)((0xFF << 24) + ((rand() % 255) << 16) + ((rand() % 255) << 8) + (rand() % 255))

void seedRandGen(); // seeds the random number generator
void replaceFwdSlash(char* str); // utility for replacing forward-slashes with back-slashes

void bitSwitch_RB(uint32_t* data, unsigned pixels); // switch red and blue bits
void bitSwitch_GB(uint32_t* data, unsigned pixels); // switch green and blue bits
void bitSwitch_RG(uint32_t* data, unsigned pixels); // switch red and green bits

// enum CHANNEL_Type getDominantChan(unsigned color);
int8_t channel_diff(uint32_t color1, uint32_t color2, CHANNEL_Type channel); // returns positive if lighter, negative if darker, zero on match
uint8_t channel_low(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t channel_hi(uint32_t color1, uint32_t color2, CHANNEL_Type channel);

uint8_t grayify8(uint32_t refColor); // produces a 8 bit grey value based on reference color
uint32_t grayify32(uint32_t refColor); // produces a 32 bit grey value based on reference color
uint8_t fract8(uint8_t refColor, double frac); // produces an 8 bit color value based on fraction of reference color
uint32_t fract32(uint32_t refColor, double frac); // produces an 32 bit color value based on fraction of reference color

uint32_t color_unique(); // produces reusable color identifier
uint32_t color_invert(uint32_t refColor); // inverts red, green, and blue values
uint32_t color_level(uint32_t color, double level); // levels color given brightness level
uint32_t colors_blend(uint32_t color1, uint32_t color2, double bVal); // blends values between two colors
uint32_t colors_fuse(uint32_t color1, uint32_t color2, double iVal); // fuse between hi and low colors

double pix_dist(unsigned p1, unsigned p2, unsigned imageWidth); // get s pixel distance based on2 offsets
unsigned pixPoint_offset(PixelPoint pixPos, ref_image_t refImage); // gets pixel offset from image coordinates
unsigned pixPoint_color(PixelPoint pixPos, ref_image_t refImage); // gets pixel color from image coordinates
unsigned pixPoint_cursorOffset(PixelPoint cursorPos, ref_image_t refImage); // gets pixel offset from cursor coordinates
unsigned pixPoint_cursorColor(PixelPoint cursorPos, ref_image_t refImage); // gets pixel color from cursor coordinates
// TODO: Add a function to calculate direction

#define RASTERON_TOOLBOX_H
#endif