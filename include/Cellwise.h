#ifndef RASTERON_CELLWISE_H

#include "Image.h"

// Pattern Image operations based on adjacent pixel values

#define NEBR_COUNT 8

enum NBR_CellFlags {
	NBR_Bot_Right = 0,
	NBR_Bot = 1,
	NBR_Bot_Left = 2,
	NBR_Right = 3,
	NBR_Left = 4,
	NBR_Top_Right = 5,
	NBR_Top = 6,
	NBR_Top_Left = 7,
	NBR_None = 8
};

typedef uint8_t nebrCheckFlags; // type used to check the existance of neighbors

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

typedef unsigned (*nebrCallback2)(unsigned, unsigned, unsigned); // accepts target and two neighbors (right/left or bottom/top)
// typedef unsigned (*nebrCallback4)(unsigned, unsigned, unsigned, unsigned); // takes bottom, right, left, and top neighbors as input, returns result color
typedef unsigned (*nebrCallback8)(unsigned, unsigned[NEBR_COUNT]); // accepts target and all neighbors in order

// Rasteron_Image* createNbrPatImg2(const Rasteron_Image* refImage, nebrCallback2 callback);
Rasteron_Image* createPatternImg_nebr(const Rasteron_Image* refImage, nebrCallback8 callback); // neighbor pattern
Rasteron_Image* createPatternImg_iter(const Rasteron_Image* refImage, nebrCallback8 callback, unsigned short iter); // iterative pattern
Rasteron_Image* createPatternImg_horz(const Rasteron_Image* refImage, nebrCallback2 callback); // horizontal scan pattern
Rasteron_Image* createPatternImg_vert(const Rasteron_Image* refImage, nebrCallback2 callback); // vertical scan pattern

// Field Image operations based on color points and distance

typedef struct {
	Rasteron_PixelPoint point;
	unsigned color;
} ColorPoint;

#define MAX_PIXEL_POS TWOPOWER(11) // 2046

typedef struct {
	ColorPoint points[MAX_PIXEL_POS];
	unsigned pointCount; // = 0;
} ColorPointTable;

void addColorPoint(ColorPointTable* table, unsigned color, double xFrac, double yFrac);

typedef unsigned (*distCallback)(unsigned color, double distance);

Rasteron_Image* createFieldImg(const Rasteron_Image* refImage, const ColorPointTable* colorPointTable, distCallback callback); // creates an image based on proximity points given a distance function
Rasteron_Image* createFieldImg_prox(const Rasteron_Image* refImage, const ColorPointTable* colorPointTable); // creates a cell layout based on proximity points

// Step Image operations based on a traced path

typedef struct {
	unsigned color;
	enum NBR_CellFlags direction;
} ColorStep;

typedef struct {
	ColorStep step;
	unsigned row;
	unsigned col;
} ColorStepRecord;

typedef ColorStep (*stepCallback)(const ColorStepRecord* prevStep);
// typedef Rasteron_Step(*stepCallback8)(const Rasteron_StepRecord* prevStep, const NebrTable* nebrTable);

Rasteron_Image* createStepImg(const Rasteron_Image* refImage, stepCallback callback); // creates an image based on steps

#define RASTERON_CELLWISE_H
#endif