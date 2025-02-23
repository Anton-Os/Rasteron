#ifndef RASTERON_FUNC_H

#include <stdint.h>

#include "support_def.h"
#include "type_def.h"

void seedRandGen(); // seeds the random number generator
void genFullFilePath(const char* name, char* fullFilePath);

uint32_t color_unique(); // produces reusable color identifier
uint32_t color_invert(uint32_t refColor); // inverts red, green, and blue values
uint32_t color_level(uint32_t color, double level); // levels color given brightness level
uint8_t channel_gray(uint32_t refColor); // produces a 8 bit grey value based on reference color
uint32_t color_gray(uint32_t refColor); // produces a 32 bit grey value based on reference color
uint8_t channel_frac(uint8_t refColor, double frac); // produces an 8 bit color value based on fraction of reference color
uint32_t color_frac(uint32_t refColor, double frac); // produces an 32 bit color value based on fraction of reference color

uint32_t swap_rb(uint32_t color);  // switch red and blue bits
uint32_t swap_gb(uint32_t color); // switch green and blue bits
uint32_t swap_rg(uint32_t color);  // switch red and green bits

// enum CHANNEL_Type dom_channel(unsigned color);
uint8_t add_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t diff_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t mult_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t pow_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t lo_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel);
uint8_t hi_channel(uint32_t color1, uint32_t color2, CHANNEL_Type channel);

uint32_t add_colors(uint32_t color1, uint32_t color2); // adds colors together
uint32_t add_rgb(uint32_t color1, uint32_t color2); // adds each channel together
uint32_t diff_colors(uint32_t color1, uint32_t color2); // subtracts smaller color from larger
uint32_t diff_rgb(uint32_t color1, uint32_t color2); // subtracts smaller channels from larger
uint32_t mult_colors(uint32_t color1, uint32_t color2); // multiplies colors together
uint32_t mult_rgb(uint32_t color1, uint32_t color2); // multiplies each channel together
uint32_t blend_colors(uint32_t color1, uint32_t color2, double bVal); // blends values between two colors
uint32_t blend_colors_eq(uint32_t color1, uint32_t color2); // blends equally between two colors
uint32_t fuse_colors(uint32_t color1, uint32_t color2, double iVal); // fuse between hi and low colors
uint32_t fuse_colors_eq(uint32_t color1, uint32_t color2); // fuse equally between hi and low colors
uint32_t root_colors(uint32_t color1, uint32_t color2); // enum CHANNEL_Type type); // multiplies colors and then takes the root
uint32_t bit_colors_and(uint32_t color1, uint32_t color2); // mix colors with and operation
uint32_t bit_colors_or(uint32_t color1, uint32_t color2); // mix colors with or operation
uint32_t bit_colors_xor(uint32_t color1, uint32_t color2); // mix colors with xor operation
// uint32_t scramble_colors(uint32_t color1, uint32_t color2, double pVal); // scrambles per channel based on probability

double pix_dist(unsigned p1, unsigned p2, unsigned imageWidth); // get s pixel distance based on2 offsets
unsigned pixPoint_offset(PixelPoint pixPos, ref_image_t refImage); // gets pixel offset from image coordinates
unsigned pixPoint_color(PixelPoint pixPos, ref_image_t refImage); // gets pixel color from image coordinates
unsigned pixPoint_cursorOffset(PixelPoint cursorPos, ref_image_t refImage); // gets pixel offset from cursor coordinates
unsigned pixPoint_cursorColor(PixelPoint cursorPos, ref_image_t refImage); // gets pixel color from cursor coordinates
// TODO: Add a function to calculate direction
void pixPoionts_expand(PixelPointTable* table, unsigned short divs);
void pixPoints_tiling(PixelPointTable* table, enum TILE_Type type, unsigned short height, unsigned short width);

nebrFlags neighbor_exists(uint32_t index, uint32_t width, uint32_t height);
uint32_t* neighbor_get(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr);
unsigned neighbor_getOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr);
unsigned short neighbor_count(unsigned color, unsigned neighbors[8]);
void neighbors_load(const NebrTable* nebrTable, unsigned* br, unsigned* b, unsigned* bl, unsigned* r, unsigned* l, unsigned* tr, unsigned* t, unsigned* tl);

#define RASTERON_FUNC_H
#endif