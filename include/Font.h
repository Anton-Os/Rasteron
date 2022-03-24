#ifndef RASTERON_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.h"

#define FONT_RESOLUTION 3000
#define FONT_PEN_OFFSET 200 // default starting pen offset
#define FONT_CANVAS_WIDTH 10000 // large enough to handle any text
#define FONT_CANVAS_HEIGHT 1000

#define FONT_INVERT_FALSE 0
#define FONT_INVERT_TRUE 1

#define FONT_SIZE_SMALL 100
#define FONT_SIZE_MED 200
#define FONT_SIZE_LARGE 300
#define FONT_SIZE_EXTRA 400
#define FONT_SIZE_GIANT 500

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
} Rasteron_FormatText;

void initFreeType(FT_Library* library);
Rasteron_Image* bakeImgTextRegScaled(FT_Library* library, const Rasteron_FormatText* textObj, unsigned scale);
Rasteron_Image* bakeImgTextReg(FT_Library* library, const Rasteron_FormatText* textObj);
Rasteron_Image* bakeImgTextInvertScaled(FT_Library* library, const Rasteron_FormatText* textObj, unsigned scale);
Rasteron_Image* bakeImgTextInvert(FT_Library* library, const Rasteron_FormatText* textObj);
void cleanupFreeType(FT_Library* library);

#define RASTERON_FONT_H
#endif