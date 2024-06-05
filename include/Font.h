#ifndef RASTERON_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "typedef.h"

#define FONT_SIZE_TINY 50
#define FONT_SIZE_SMALL 150
#define FONT_SIZE_MED 250
#define FONT_SIZE_LARGE 350
#define FONT_SIZE_XL 500

// internal macros
#define FONT_RESOLUTION 1500
#define FONT_PEN_OFFSET 100 // default starting pen offset

typedef struct { unsigned xMin; unsigned xMax; unsigned yMin;  unsigned yMax; } TextSize;

typedef struct {
    const char* fontFile; // file used as font
    const char* text; // text to display
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Text;

#define MAX_MESSAGE_LINES 64

#define TEXT_ALIGN_LEFT -1.0
#define TEXT_ALIGN_CENTER 0.0
#define TEXT_ALIGN_RIGHT 1.0

typedef struct {
    const char* fontFile; // file used as font
    const char* messages[MAX_MESSAGE_LINES]; // texts to display
    double alignment; // used which side the text is focused
    unsigned messageCount; // number of lines
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Message;

extern FT_Library _freetypeLib; // internal freetype library

void initFreeType();
void cleanupFreeType();

Rasteron_Image* textImgOp(const Rasteron_Text* textObj, unsigned size); // TODO: Needs to be optimized!
Rasteron_Image* textPadImgOp(const Rasteron_Text* textObj, unsigned size, unsigned short padding[4]);
Rasteron_Image* messageImgOp(const Rasteron_Message* messageObj, unsigned size); // TODO: Needs to be optimized!

#define RASTERON_FONT_H
#endif