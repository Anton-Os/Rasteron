#ifndef RASTERON_CELLWISE_H

#include "Image.h"
#include "pixelpoint.h"

#define NEBR_COUNT 8
#define NEBR_RANDOM (rand() % 8)

enum NEBR_CellFlag {
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

typedef uint8_t nebrFlags; // type to check existance of neighbors

typedef struct {
    const uint32_t* target;
    uint32_t** nebrs;
    nebrFlags flags; // 1 or 0 depending on if neighbor exists or not
} NebrTable;

typedef struct {
	uint32_t count;
	NebrTable* tables;
} NebrTable_List;

NebrTable_List* genNebrTables(const Rasteron_Image* image);
void delNebrTables(NebrTable_List* nebrTables);

// Pattern Image operations based on adjacent pixel values

typedef unsigned (*nebrCallback2)(unsigned, unsigned[2]); // accepts target and two neighbors
typedef unsigned (*nebrCallback8)(unsigned, unsigned[8]); // accepts target and all neighbors

Rasteron_Image* createPatternImg(const Rasteron_Image* refImage, nebrCallback8 callback); // neighbor pattern
Rasteron_Image* createMultiPatternImg(const Rasteron_Image* refImage, nebrCallback8 callback, unsigned short iter); // iterative pattern
Rasteron_Image* createPatternImg_horz(const Rasteron_Image* refImage, nebrCallback2 callback); // horizontal pattern
Rasteron_Image* createPatternImg_vert(const Rasteron_Image* refImage, nebrCallback2 callback); // vertical pattern

// Field Image operations based on color points and distance

typedef unsigned (*distCallback)(unsigned color, double distance);

Rasteron_Image* createFieldImg(ImageSize size, const ColorPointTable* colorPointTable, distCallback callback); // creates image based on color points given a distance function
Rasteron_Image* createFieldImg_vornoi(ImageSize size, const ColorPointTable* colorPointTable); // creates voronoi cell image

// Step Image operations based on a traced path

#define MAX_COLOR_STEPS 64

typedef struct {
	unsigned color;
	enum NEBR_CellFlag direction;
} ColorStep;

typedef ColorStep (*stepCallback)(const NebrTable* nebrTable, ColorStep prevStep, unsigned short stepCount);

Rasteron_Image* createStepImg(const Rasteron_Image* refImage, const PixelPointTable* pixelPointTable, stepCallback callback); // creates image based on tracing paths

#define RASTERON_CELLWISE_H
#endif