#ifndef RASTERON_INIT_H

#include "typedef.h"

// --------------------------------   Image    -------------------------------- //

Rasteron_Image* alloc_image(const char* name, uint32_t height, uint32_t width);
#define RASTERON_ALLOC(name, height, width) (Rasteron_Image*)(alloc_image(name, height, width))

void dealloc_image(Rasteron_Image* image);
#define RASTERON_DEALLOC(image) (dealloc_image(image))


// --------------------------------   Swatch    -------------------------------- //

ColorSwatch createSwatch(unsigned color, uint8_t deviation);


// --------------------------------   PixelPoint & ColorPoint    -------------------------------- //

void pixelPointToTable(PixelPointTable* table, double xFrac, double yFrac);
void colorPointToTable(ColorPointTable* table, unsigned color, double xFrac, double yFrac);

typedef unsigned (*colorPointCallback)(double x, double y); // return NO_COLOR or color for color point

// ColorPointTable loadColorPtTable_regular(unsigned color, unsigned short rows, unsigned short cols);
// ColorPointTable loadColorPtTable_irregular(unsigned color, float chance);
// ColorPointTable loadColorPtTable_custom(ImageSize size, colorPointCallback);

// --------------------------------   Cellwise    -------------------------------- //

NebrTable_List* loadNebrTables(ref_image_t refImage);
void delNebrTables(NebrTable_List* nebrTables);

nebrFlags checkExistNebrs(uint32_t index, uint32_t width, uint32_t height);
unsigned findNeighborOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr);
uint32_t* findNeighbor(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr);



// --------------------------------   Callbacks    -------------------------------- //

typedef unsigned (*recolorCallback)(unsigned color);
typedef unsigned (*mixCallback)(unsigned color1, unsigned color2);
typedef unsigned (*coordCallback)(double x, double y);
typedef unsigned (*fieldCallback)(unsigned color, double distance);
typedef unsigned (*fieldCallback3)(unsigned colors[3], double distances[3]);

typedef unsigned (*nebrCallback2)(unsigned, unsigned[2]); // target and 2 neighbors
typedef unsigned (*nebrCallback4)(unsigned, unsigned[4]); // target and 4 neighbors
typedef unsigned (*nebrCallback8)(unsigned, unsigned[8]); // target and all 8 neighbors


#define RASTERON_INIT_H
#endif