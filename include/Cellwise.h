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


NebrTable_List* genNebrTables(Rasteron_Image* image);
void delNebrTables(NebrTable_List* nebrTables);

typedef unsigned (*fourNebrCallback)(unsigned, unsigned, unsigned, unsigned); // takes bottom, right, left, and top neighbors as input, returns result color
typedef unsigned (*eightNebrCallback)(unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned, unsigned); // takes all neighbors in order as input, returns result color

Rasteron_Image* createPatternImg4(const Rasteron_Image* refImage, fourNebrCallback callback);
Rasteron_Image* createPatternImg8(const Rasteron_Image* refImage, eightNebrCallback callback);

#define RASTERON_CELLWISE_H
#endif