#ifndef RASTERON_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.h"

#define FONT_RES 3000
#define FONT_SCALE 100 // default scaling used
#define FONT_PEN_OFF 200 // default starting pen offset

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