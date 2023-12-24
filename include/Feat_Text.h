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
#define FONT_RESOLUTION 2000
#define FONT_PEN_OFFSET 200 // default starting pen offset
#define FONT_CANVAS_WIDTH 10000 // large enough to handle any text
#define FONT_CANVAS_HEIGHT 1000

#ifdef __cplusplus
    #define FONT_INVERT 1
#else
    #define FONT_INVERT 0
#endif

typedef struct { unsigned xMin; unsigned xMax; unsigned yMin;  unsigned yMax; } TextSize;

typedef struct {
    const char* fontFile; // file used as font
    const char* text; // text to display
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Text;

#define MAX_MESSAGE_LINES 64

typedef struct {
    const char* fontFile; // file used as font
    const char* messages[MAX_MESSAGE_LINES]; // texts to display
    unsigned messageCount; // number of lines
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Message;

extern int _invertFont;
extern FT_Library _freetypeLib; // internal freetype library
// static unsigned _textSize = FONT_SIZE_MED; // text size type

void initFreeType();
void cleanupFreeType();

Rasteron_Image* textImgOp(const Rasteron_Text* textObj, unsigned size); // regular
Rasteron_Image* messageImgOp(const Rasteron_Message* messageObj, unsigned size);

#define RASTERON_FONT_H
#endif