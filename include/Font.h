#ifndef RASTERON_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.h"
#define FONT_RESOLUTION 3000
#define FONT_PEN_OFFSET 200 // default starting pen offset
#define FONT_CANVAS_WIDTH 10000 // large enough to handle any text
#define FONT_CANVAS_HEIGHT 1000

// custom sizes sizes
#define FONT_SIZE_SMALL 100
#define FONT_SIZE_MED 200
#define FONT_SIZE_LARGE 300
#define FONT_SIZE_EXTRA 400
#define FONT_SIZE_GIANT 500

#define FONT_REGULAR 0
#define FONT_INVERTED 1

typedef struct {
    unsigned xMin;
    unsigned xMax;
    unsigned yMin;
    unsigned yMax;
} Rasteron_TextSizeProperties;

typedef struct {
    const char* fileName; // file used as font
    const char* text; // text to display
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Text;

FT_Library _freetypeLib; // internal freetype library

void initFreeType();
Rasteron_Image* bakeText_sized(const Rasteron_Text* textObj, unsigned scale); // regular sized
Rasteron_Image* bakeText(const Rasteron_Text* textObj); // regular
Rasteron_Image* bakeTextI_sized(const Rasteron_Text* textObj, unsigned scale); // inverted sized
Rasteron_Image* bakeTextI(const Rasteron_Text* textObj); // inverted
void cleanupFreeType();

#define RASTERON_FONT_H
#endif