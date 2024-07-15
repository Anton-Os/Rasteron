#ifndef RASTERON_TYPE_H

#include "support_def.h"

// --------------------------------   Image    -------------------------------- //
//
//		Most important type representing images plus helper struct for sizing
//		and enums for mirroring and cropping images

#define INVERT_IMG_FALSE 0
#define INVERT_IMG_TRUE 1

typedef struct { uint32_t height; uint32_t width; } ImageSize;

typedef struct {
    char* name;
	uint32_t height;
    uint32_t width;
    uint32_t* data;
} Rasteron_Image;

typedef const Rasteron_Image *const ref_image_t;

ImageSize internal_create_size(unsigned height, unsigned width);
#define RASTERON_SIZE(height, width) (ImageSize)(internal_create_size(height, width))

Rasteron_Image* internal_alloc_img(const char* name, uint32_t height, uint32_t width);
#define RASTERON_ALLOC(name, height, width) (Rasteron_Image*)(internal_alloc_img(name, height, width))

void internal_dealloc_img(Rasteron_Image* image);
#define RASTERON_DEALLOC(image) (internal_dealloc_img(image))

// --------------------------------   Swatch    -------------------------------- //

enum SWATCH_Colors {
    SWATCH_Yellow_Add = 0, SWATCH_Cyan_Add = 1, SWATCH_Magenta_Add = 2, 
	SWATCH_Light = 3, SWATCH_Dark = 4,
	SWATCH_Yellow_Sub = 5, SWATCH_Cyan_Sub = 6, SWATCH_Magenta_Sub = 7,
};

typedef struct {
    unsigned base; // swatch base
    unsigned colors[8]; // generated swatch colors
    uint8_t deviation;
} ColorSwatch;

ColorSwatch createSwatch(unsigned color, uint8_t deviation);

// --------------------------------   PixelPoint & ColorPoint    -------------------------------- //

#define MAX_PIXELPOINTS 40000

typedef struct { double x; double y; } PixelPoint; // x and y fractional offset

typedef struct {
	PixelPoint points[MAX_PIXELPOINTS];
	unsigned pointCount;
} PixelPointTable;

typedef struct { double x; double y; unsigned color; } ColorPoint; // x and y fractional offset with color

typedef struct {
	ColorPoint points[MAX_PIXELPOINTS];
	unsigned pointCount;
} ColorPointTable;

void pixelPointToTable(PixelPointTable* table, double xFrac, double yFrac);
void colorPointToTable(ColorPointTable* table, unsigned color, double xFrac, double yFrac);

typedef unsigned (*colorPointCallback)(double x, double y); // return NO_COLOR or color for color point

// --------------------------------  Noise    -------------------------------- //

#define MAX_grid 256

typedef struct {
	unsigned xCells; unsigned yCells;
	unsigned color1; unsigned color2;
} ColorGrid;

typedef struct {
    ColorGrid grids[MAX_grid];
	unsigned gridCount;
} ColorGridTable;


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

NebrTable_List* loadNebrTables(ref_image_t refImage);
void delNebrTables(NebrTable_List* nebrTables);

// --------------------------------   Enums    -------------------------------- //

typedef enum { CHANNEL_Alpha = -1, CHANNEL_Red = 0, CHANNEL_Green = 1, CHANNEL_Blue = 2 } CHANNEL_Type;
enum FLIP_Type { FLIP_None = -1, FLIP_Clock = 0, FLIP_Counter = 1, FLIP_Upside = 2 };
enum TILE_Type { TILE_Square = 0, TILE_Triangle = 1, TILE_Hexagon = 2, TILE_Diagonal = 3, TILE_Perimiter = 4 };
enum SIDE_Type { SIDE_None = -1, SIDE_Top = 0, SIDE_Bottom = 1, SIDE_Left = 2, SIDE_Right = 3, SIDE_Radial = 4 };

// --------------------------------   Callbacks    -------------------------------- //

typedef unsigned (*recolorCallback)(unsigned color);

typedef unsigned (*mixCallback)(unsigned color1, unsigned color2);
typedef unsigned (*mixCallback4)(unsigned color1, unsigned color2, unsigned color3, unsigned color4);
typedef unsigned (*coordCallback)(double x, double y);
typedef unsigned (*fieldCallback)(unsigned color, double distance, PixelPoint pixelPoint);
typedef unsigned (*fieldCallback3)(unsigned colors[3], double distances[3], PixelPoint pixPoints[3]);

typedef unsigned (*nebrCallback2)(unsigned, unsigned[2]); // target and 2 neighbors
typedef unsigned (*nebrCallback4)(unsigned, unsigned[4]); // target and 4 neighbors
typedef unsigned (*nebrCallback8)(unsigned, unsigned[8]); // target and all 8 neighbors


#define RASTERON_TYPE_H
#endif
