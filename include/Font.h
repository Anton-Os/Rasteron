#include <ft2build.h>
#include FT_FREETYPE_H

#include "Image.h"

typedef struct {
    uint32_t xOffset;
    // uint32_t yOffset;
    uint32_t width;
    uint32_t height;
} Rasteron_TextSizeParams;

typedef struct {
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
    const char* fontFileName; // file used as font
    const char* text; // text to display
} Rasteron_FormatText;

void initFreeType(FT_Library* library);
Rasteron_Image* bakeImgText(const Rasteron_FormatText* text, FT_Library* library, int x, int y);
void cleanupFreeType(FT_Library* library);