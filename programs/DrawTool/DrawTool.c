#define CANVAS_COLOR 0xFF111111
#define COLOR_POINTS 4
#define OSCILATION 30.0
#define DOT_RADIUS 0.05
#define FIELD_PRODUCT 5.0
#define STROKE_MOD 5.0

static unsigned dotSize = 1.0;
static unsigned isCoords = CANVAS_COLOR;
static unsigned xColor = 0xFFFFFF00;
static unsigned yColor = 0xFFFF00FF;
static unsigned short mode = '1';
static unsigned short pressCount = 0;

#include "../_Prog.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

#include "Draw.c"

// Overriden Functions

void setup(char input){ // double (*xMod)(double), double (*yMod)(double)){
    Rasteron_Image* stagingImg = NULL;

    switch(input){
    case 'q': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw1); break;
    case 'w': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw2); break;
    case 'e': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw3); break;
    case 'r': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw4); break;
    case 't': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw5); break;
    case 'y': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw6); break;
    case 'u': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw7); break;
    case 'i': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw8); break;
    case 'o': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw9); break;
    case 'p': stagingImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw0); break;
    case 'a': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw1); break;
    case 's': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw2); break;
    case 'd': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw3); break;
    case 'f': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw4); break;
    case 'g': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw5); break;
    case 'h': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw6); break;
    case 'j': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw7); break;
    case 'k': stagingImg = brushDrawImgOp((ImageSize){1024, 1024}, brushDraw8); break;
    case 'z': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw1); break;
    case 'x': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw2); break;
    case 'c': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw3); break;
    case 'v': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw4); break;
    case 'b': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw5); break;
    case 'n': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw6); break;
    case 'm': stagingImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw7); break;
    }

    if(stagingImg != NULL) 
      for(unsigned p = 0; p < stagingImg->width * stagingImg->height; p++) 
        if(*(stagingImg->data + p) != NO_COLOR) *(_outputImg->data + p) = *(stagingImg->data + p);
}

void _onKeyEvent(char key){
    if(colorPointTable.pointCount > COLOR_POINTS && key != 13) mode = key;

    switch(key){
        case 'z': lineEq = &basicLine; break;
        case 'x': lineEq = &wobbleLine; break;
        case 'c': lineEq = &powerLine; break;
        case 'v': lineEq = &squigLine; break;
        case 'b': lineEq = &cutLine; break;
        case 'n': lineEq = &bulge; break;
        case 'm': lineEq = &waves; break;
    }
    setup(mode); // xMod, yMod);
}

void _onPressEvent(double x, double y){
    xColor = RAND_COLOR(); // | ((unsigned)(x * 256) * 0x10001);
    yColor = color_invert(xColor); // | ((unsigned)(y * 256) * 0x101);

    pixelPointToTable(&pixelPointTable, x, y);
    // colorPointToTable(&colorPointTable, (colorPointTable.pointCount % 2 == 0)? xColor : yColor, x, y);
    colorPointToTable(&colorPointTable, color_level((colorPointTable.pointCount % 2 == 0)? xColor : yColor, (x + y) / 2.0), x, y);

    setup(mode);
    pressCount++;
}

void _onTickEvent(unsigned secs){}

// Generative Function

Rasteron_Image* drawTool(char* args) {
    return checkeredImgOp((ImageSize){ 1024, 1024 }, (ColorGrid){ 10, 10, 0xFF333333, 0xFFEEEEEE });
}

// Executable Function

int main(int argc, char** argv){
    pixelPointTable.pointCount = 0;
    colorPointTable.pointCount = 0;

    _run(argc, argv, drawTool); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}