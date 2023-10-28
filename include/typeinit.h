#ifndef RASTERON_INIT_H

#include "type.h"

// --------------------------------   Image    -------------------------------- //

Rasteron_Image* alloc_image(const char* name, uint32_t height, uint32_t width);
void free_image(Rasteron_Image* image);

// --------------------------------   Callbacks    -------------------------------- //

typedef unsigned (*nebrCallback2)(unsigned, unsigned[2]); // target and 2 neighbors
typedef unsigned (*nebrCallback4)(unsigned, unsigned[8]); // target and 4 neighbors
typedef unsigned (*nebrCallback8)(unsigned, unsigned[8]); // target and all 8 neighbors
typedef unsigned (*recolorCallback)(unsigned color);
typedef unsigned (*dualcolorCallback)(unsigned color1, unsigned color2);
typedef unsigned (*mapCallback)(double x, double y);
typedef unsigned (*fieldCallback)(unsigned color, double distance);


// --------------------------------   Seed    -------------------------------- //

void seedToTable(ColorSeedTable* table, unsigned color);
void seedweightToTable(ColorSeedTable* table, unsigned color, double weight);

// --------------------------------   Swatch    -------------------------------- //

ColorSwatch createSwatch(unsigned color, uint8_t deviation);

// --------------------------------   PixelPoint & ColorPoint    -------------------------------- //

void pixelPointToTable(PixelPointTable* table, double xFrac, double yFrac);
void colorPointToTable(ColorPointTable* table, unsigned color, double xFrac, double yFrac);

// --------------------------------   Sprite    -------------------------------- //

Rasteron_Sprite* loadSprite(const Rasteron_Image* refImage);
void free_sprite(Rasteron_Sprite* sprite);

// --------------------------------   Heightmap    -------------------------------- //

Rasteron_Heightmap* alloc_heightmap(uint32_t height, uint32_t width, double minBound, double maxBound);
Rasteron_Heightmap* loadHeightmap(ref_image_t ref); // create heightmap from an image file
void free_heightmap(Rasteron_Heightmap* heightmap);

// --------------------------------   Cellwise    -------------------------------- //

NebrTable_List* loadNebrTables(ref_image_t refImage);
void delNebrTables(NebrTable_List* nebrTables);

nebrFlags checkExistNebrs(uint32_t index, uint32_t width, uint32_t height);
unsigned findNeighborOffset(unsigned width, unsigned offset, enum NEBR_CellFlag whichNebr);
uint32_t* findNeighbor(Rasteron_Image* refImage, uint32_t index, enum NEBR_CellFlag whichNebr);

#define RASTERON_INIT_H
#endif