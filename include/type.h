#ifndef RASTERON_TYPE_H

#include "support_def.h"

// --------------------------------   Image    -------------------------------- //
//
//		Most important type representing images plus helper struct for sizing
//		and enums for mirroring and cropping images

typedef struct { uint32_t height; uint32_t width; } ImageSize;

typedef struct {
    char* name;
    uint32_t width;
	uint32_t height;
    uint32_t* data;
} Rasteron_Image;

typedef const Rasteron_Image *const ref_image_t;


// --------------------------------   Seed    -------------------------------- //

#define MAX_SEEDS 2046
#define DEFAULT_SEED_WEIGHT 0.0

typedef struct {
	unsigned color;
	double weight;
} ColorSeed;

typedef struct {
    ColorSeed seeds[2046];
	unsigned seedCount; // = 0;
} ColorSeedTable;


// --------------------------------   Swatch    -------------------------------- //

enum SWATCH_Colors {
    SWATCH_Yellow_Add = 0, SWATCH_Cyan_Add = 1, SWATCH_Magenta_Add = 2, 
	SWATCH_Light = 3,
	SWATCH_Dark = 4,
	SWATCH_Yellow_Sub = 5, SWATCH_Cyan_Sub = 6, SWATCH_Magenta_Sub = 7,
};

typedef struct {
    unsigned base; // swatch base
    unsigned colors[8]; // generated swatch colors
    uint8_t deviation;
} ColorSwatch;


// --------------------------------   PixelPoint & ColorPoint    -------------------------------- //

typedef struct { double x; double y; } PixelPoint; // x and y fractional offset

typedef struct {
	PixelPoint points[2046];
	unsigned pointCount;
} PixelPointTable;

typedef struct { double x; double y; unsigned color; } ColorPoint; // x and y fractional offset with color

typedef struct {
	ColorPoint points[2046];
	unsigned pointCount;
} ColorPointTable;

// --------------------------------  Noise    -------------------------------- //

typedef struct {
	unsigned xCells; unsigned yCells;
	unsigned color1; unsigned color2;
} ColorLattice;

typedef struct {
    ColorLattice lattices[2046];
	unsigned latticeCount;
} ColorLatticeTable;


// --------------------------------   Cellwise    -------------------------------- //

enum NEBR_CellFlag {
	NEBR_Bot_Right = 0, NEBR_Bot = 1, NEBR_Bot_Left = 2,
	NEBR_Right = 3, NEBR_Left = 4,
	NEBR_Top_Right = 5, NEBR_Top = 6, NEBR_Top_Left = 7,
	NEBR_None = 8
};

typedef uint8_t nebrFlags; // checks existence of neighbors

typedef struct {
    const uint32_t* target;
    uint32_t** nebrs;
    nebrFlags flags; // 1 or 0 if neighbor exists
} NebrTable;

typedef struct {
	uint32_t count;
	NebrTable* tables;
} NebrTable_List;

// --------------------------------   Other    -------------------------------- //

enum FLIP_Type { FLIP_None = -1, FLIP_Clock = 0, FLIP_Counter = 1, FLIP_Upside = 2 };
enum CROP_Type { CROP_None = -1, CROP_Top = 0, CROP_Bottom = 1, CROP_Left = 2, CROP_Right = 3 };

#define RASTERON_TYPE_H
#endif
