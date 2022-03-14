#ifndef RASTERON_CELLWISE_H

#include "Image.h"

// Start from zero to index arrays easily
enum NBR_CellFlags {
	NBR_Bot_Right = 0,
	NBR_Bot = 1,
	NBR_Bot_Left = 2,
	NBR_Right = 3,
	NBR_Left = 4,
	NBR_Top_Right = 5,
	NBR_Top = 6,
	NBR_Top_Left = 7,
};

typedef uint8_t nebrCheckFlags; // A number with the bitflags set above

typedef struct {
    const uint32_t* target;
    uint32_t** nebrs;
    nebrCheckFlags nebrExistFlags;
    // unsigned short nebrCount;
} NebrTable;

typedef struct {
	uint32_t count;
	NebrTable* tables;
} NebrTable_List;


NebrTable_List* genNebrTables(const Rasteron_Image* image);
void delNebrTables(NebrTable_List* nebrTables);

typedef unsigned (*nebrCallback2)(unsigned, unsigned); // takes only right and left neighbors as input, returns result color
typedef unsigned (*nebrCallback4)(unsigned, unsigned, unsigned, unsigned); // takes bottom, right, left, and top neighbors as input, returns result color
typedef unsigned (*nebrCallback8)(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned); // takes all neighbors in order as input, returns result color

Rasteron_Image* createCellPatImg2(const Rasteron_Image* refImage, nebrCallback2 callback);
Rasteron_Image* createIterPatImg2(const Rasteron_Image* refImage, nebrCallback2 callback, unsigned short iter);
Rasteron_Image* createCellPatImg4(const Rasteron_Image* refImage, nebrCallback4 callback);
Rasteron_Image* createIterPatImg4(const Rasteron_Image* refImage, nebrCallback2 callback, unsigned short iter);
Rasteron_Image* createCellPatImg8(const Rasteron_Image* refImage, nebrCallback8 callback);
Rasteron_Image* createIterPatImg8(const Rasteron_Image* refImage, nebrCallback2 callback, unsigned short iter);
Rasteron_Image* createHorzPatImg(const Rasteron_Image* refImage, nebrCallback2 callback); // horizontal scan pattern
Rasteron_Image* createVertPatImg(const Rasteron_Image* refImage, nebrCallback2 callback); // vertical scan pattern

#define RASTERON_CELLWISE_H
#endif