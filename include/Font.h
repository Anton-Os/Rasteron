#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.h"

#define FONT_RES 3000
#define FONT_PEN_OFF 200 // default starting pen offset

typedef struct {
    unsigned xMin;
    unsigned xMax;
    unsigned yMin;
    unsigned yMax;
} Rasteron_TextSizeParams;

typedef struct {
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
    const char* fontFileName; // file used as font
    const char* text; // text to display
} Rasteron_FormatText;

void initFreeType(FT_Library* library);
Rasteron_Image* bakeImgText(const Rasteron_FormatText* text, FT_Library* library, unsigned scale);
void cleanupFreeType(FT_Library* library);