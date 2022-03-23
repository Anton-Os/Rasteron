#ifndef RASTERON_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.h"

#ifndef INVERT_FONT_AXIS
#define INVERT_FONT_AXIS 0 // font drawing is not inverted by default!
#endif

#define FONT_RESOLUTION 3000
#define FONT_SCALE 200 // test scaling
#define FONT_PEN_OFFSET 200 // default starting pen offset
#define FONT_CANVAS_WIDTH 10000 // large enough to handle any text
#define FONT_CANVAS_HEIGHT 1000

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
Rasteron_Image* bakeImgTextScaled(FT_Library* library, const Rasteron_FormatText* textObj, unsigned scale);
Rasteron_Image* bakeImgText(FT_Library* library, const Rasteron_FormatText* textObj);
void cleanupFreeType(FT_Library* library);

#define RASTERON_FONT_H
#endif