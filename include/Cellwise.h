#ifndef IMAGE_LOADER_H
    #include "Loader.h"
#endif

/* #define BOT_RIGHT_NBR_FLAG 1
#define BOT_NBR_FLAG 2
#define BOT_LEFT_NBR_FLAG 4
#define LEFT_NBR_FLAG 8
#define RIGHT_NBR_FLAG 16
#define TOP_RIGHT_NBR_FLAG 32
#define TOP_NBR_FLAG 64
#define TOP_LEFT_NBR_FLAG 128 */

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

#define MIN_COLOR_BOUND 0x00
#define MAX_COLOR_BOUND 0xFF


// UB and LB stand for Upper Bound and Lower Bound, respectively
enum NBR_ColorRange_Mode {
    NBR_LaysBetween = 1,
    NBR_LaysOutside = 2,
    NBR_LaysAboveUB = 3,
    NBR_LaysBelowUB = 4,
    NBR_LaysAboveLB = 5,
    NBR_LaysBelowLB = 6,
};

typedef struct {
    uint8_t bLowerBound; // Black lower bound
    uint8_t wUpperBound; // White upper bound
} PixelCRange_BW;

typedef struct {
    uint8_t rLowerBound; // Red lower bound
    uint8_t rUpperBound; // Red upper bound
    uint8_t gLowerBound; // Green lower bound 
    uint8_t gUpperBound; // Green upper bound 
    uint8_t bLowerBound; // Blue lower bound
    uint8_t bUpperBound; // Blue upper bound 
    uint8_t aLowerBound; // Alpha lower bound 
    uint8_t aUpperBound; // Alpha upper bound 
} PixelCRange_RGBA;


/* Code for heightmap generation later on */
typedef struct {
    float baseWeight;
    float* compoundWeights;
    unsigned short compoundIndices;
    float clampLower, clampUpper;
} PixelWeightTable;

typedef struct {
    const PixelWeightTable* refTable;
    float weight;
} PixelWeight;

//nebrCheckFlags check_colorRangeNebrs_BW(PixelCRange_BW pixCRange, uint8_t greyRef, enum NBR_ColorRange_Mode mode);
//nebrCheckFlags check_colorRangeNebrs_RGBA(PixelCRange_RGBA pixCRange, uint32_t colorRef, enum NBR_ColorRange_Mode mode);

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

// Checks whether cell neighbors exist
nebrCheckFlags check_existNebrs(uint32_t index, uint32_t imgWidth, uint32_t imgHeight);

// NebrTable* gen_nebrTables(const uint32_t* raster, uint32_t imgWidth, uint32_t imgHeight);
NebrTable_List* gen_nebrTables(const uint32_t* raster, uint32_t imgWidth, uint32_t imgHeight);
void print_nebrTables(const NebrTable_List* nebrTables); // For debug purposes
void del_nebrTables(NebrTable_List* nebrTables);