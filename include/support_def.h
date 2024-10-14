
#ifndef RASTERON_SUPPORT_H

#define ASSETS_DIR "F:/Codex/PROJECT/Rasteron/master/assets"
#define MATERIAL_ICONS_FILE "F:/Codex/PROJECT/Rasteron/master/assets/material-icons-png/_MaterialIconList.txt"

#define USE_IMG_TIFF // Enable TIFF Support
#define USE_IMG_PNG // Enable PNG Support
#define USE_IMG_BMP // Enable BMP Support

#define NO_COLOR 0x00000000

#define BLACK_COLOR 0xFF000000 // level 0
#define DARK_COLOR 0xFF333333 // level 1
#define MID_LO_COLOR 0xFF666666 // level 2
#define MID_HI_COLOR 0xFF999999 // level 3
#define LIGHT_COLOR 0xFFCCCCCC // level 4
#define WHITE_COLOR 0xFFFFFFFF // level 5

#define ALPHA_CHANNEL 0xFF000000 // alpha channel mask from first two bytes
#define RED_CHANNEL 0x00FF0000 // red channel mask from second two bytes
#define GREEN_CHANNEL 0x0000FF00 // green channel mask from third two bytes
#define BLUE_CHANNEL 0x000000FF // blue channel mask from last two bytes

#define TWOPOWER(x) (1 << (x))

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <math.h>

#ifndef RASTERON_TYPE_H
#include "type_def.h"
#endif

#define RAND_COLOR() (uint32_t)((0xFF << 24) + ((rand() % 255) << 16) + ((rand() % 255) << 8) + (rand() % 255))

void seedRandGen(); // seeds the random number generator

uint32_t color_unique(); // produces reusable color identifier
uint32_t color_invert(uint32_t refColor); // inverts red, green, and blue values
uint32_t color_level(uint32_t color, double level); // levels color given brightness level
uint8_t channel_grayscale(uint32_t refColor); // produces a 8 bit grey value based on reference color
uint32_t color_grayscale(uint32_t refColor); // produces a 32 bit grey value based on reference color
uint8_t channel_fractional(uint8_t refColor, double frac); // produces an 8 bit color value based on fraction of reference color
uint32_t color_fractional(uint32_t refColor, double frac); // produces an 32 bit color value based on fraction of reference color

void bitSwitch_RB(uint32_t* data, unsigned pixels); // switch red and blue bits
void bitSwitch_GB(uint32_t* data, unsigned pixels); // switch green and blue bits
void bitSwitch_RG(uint32_t* data, unsigned pixels); // switch red and green bits
// enum CHANNEL_Type getDominantChan(unsigned color);
int8_t channel_diff(uint32_t color1, uint32_t color2, CHANNEL_Type channel); // returns positive if lighter, negative if darker, zero on match
uint8_t channel_low(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t channel_hi(uint32_t color1, uint32_t color2, CHANNEL_Type channel);

uint32_t colors_diff(uint32_t color1, uint32_t color2); // subtracts smaller color from larger
uint32_t colors_blend(uint32_t color1, uint32_t color2, double bVal); // blends values between two colors
uint32_t colors_fuse(uint32_t color1, uint32_t color2, double iVal); // fuse between hi and low colors
uint32_t colors_powroot(uint32_t color1, uint32_t color2); // enum CHANNEL_Type type); // multiplies colors and then takes the root
uint32_t colors_scramble(uint32_t color1, uint32_t color2, double pVal); // scrambles per channel based on probability

double pix_dist(unsigned p1, unsigned p2, unsigned imageWidth); // get s pixel distance based on2 offsets
unsigned pixPoint_offset(PixelPoint pixPos, ref_image_t refImage); // gets pixel offset from image coordinates
unsigned pixPoint_color(PixelPoint pixPos, ref_image_t refImage); // gets pixel color from image coordinates
unsigned pixPoint_cursorOffset(PixelPoint cursorPos, ref_image_t refImage); // gets pixel offset from cursor coordinates
unsigned pixPoint_cursorColor(PixelPoint cursorPos, ref_image_t refImage); // gets pixel color from cursor coordinates
// TODO: Add a function to calculate directio
void pixPoionts_expand(PixelPointTable* table, unsigned short divs);
void pixPoints_tiling(PixelPointTable* table, enum TILE_Type type, unsigned short height, unsigned short width);

nebrFlags neighbor_exists(uint32_t index, uint32_t width, uint32_t height);
uint32_t* neighbor_get(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr);
unsigned neighbor_getOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr);
unsigned short neighbor_count(unsigned color, unsigned neighbors[8]);
void neighbors_load(const NebrTable* nebrTable, unsigned* br, unsigned* b, unsigned* bl, unsigned* r, unsigned* l, unsigned* tr, unsigned* t, unsigned* tl);


#define RASTERON_SUPPORT_H
#endif
