#ifndef RASTERON_FONT_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "type.h"

#define FONT_SIZE_SMALL 100
#define FONT_SIZE_MED 200
#define FONT_SIZE_LARGE 300
#define FONT_SIZE_EXTRA 400
#define FONT_SIZE_GIANT 500

#define FONT_REGULAR 0
#define FONT_INVERTED 1

// internal macros
#define FONT_RESOLUTION 3000
#define FONT_PEN_OFFSET 200 // default starting pen offset
#define FONT_CANVAS_WIDTH 10000 // large enough to handle any text
#define FONT_CANVAS_HEIGHT 1000

typedef struct {
    unsigned xMin;
    unsigned xMax;
    unsigned yMin; 
    unsigned yMax;
} TextSize;

typedef struct {
    const char* fileName; // file used as font
    const char* text; // text to display
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Text;

#define MAX_MESSAGE_LINES 64

typedef struct {
    const char* fileName; // file used as font
    const char* message[MAX_MESSAGE_LINES]; // texts to display
    unsigned lineCount; // number of lines
    uint32_t bkColor; // background color
    uint32_t fgColor; // foreground color
} Rasteron_Message;

static FT_Library _freetypeLib; // internal freetype library
// static unsigned _textSize = FONT_SIZE_MED; // text size type

void initFreeType();
void cleanupFreeType();

Rasteron_Image* bakeText(const Rasteron_Text* textObj, unsigned size); // regular
Rasteron_Image* bakeMessage(const Rasteron_Message* messageObj, unsigned size); // TODO: Implement
Rasteron_Image* bakeTextI(const Rasteron_Text* textObj, unsigned size); // inverted
Rasteron_Image* bakeMessageI(const Rasteron_Message* messageObj, unsigned size); // TODO: Implement

#define RASTERON_FONT_H
#endif