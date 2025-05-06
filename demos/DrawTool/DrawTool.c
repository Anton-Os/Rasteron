#include "../_Catalouge.h"

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

#include "../_Demo.h"

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

#include "Draw.c"

void setup(char input){ // double (*xMod)(double), double (*yMod)(double)){
    if(!isdigit(input))
        switch(input){
            case 'q': case 'w': case 'e': case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p':
            case 'z': case 'x': case 'c': case 'v': case 'b': case 'n': case 'm':
                Rasteron_Image* tempImg = mapImgOp((ImageSize){1024, 1024}, strokeDraw);
                for(unsigned p = 0; p < tempImg->width * tempImg->height; p++)
                    if(*(tempImg->data + p) != NO_COLOR) *(_outputImg->data + p) = *(tempImg->data + p);
                RASTERON_DEALLOC(tempImg);
            break;
            case 'a': case 's': case 'd': case 'f': case 'g': case 'h': case 'j': case 'k': case 'l':
                RASTERON_DEALLOC(_outputImg);
                _outputImg = fieldExtImgOp((ImageSize){ 1024, 1024 }, &colorPointTable, fieldDraw);
            break;
        }
    else {
        Rasteron_Image* tempImg = fieldImgOp((ImageSize){1024, 1024}, &colorPointTable, dotDraw);
        for(unsigned p = 0; p < tempImg->width * tempImg->height; p++)
            if(*(tempImg->data + p) != NO_COLOR) *(_outputImg->data + p) = *(tempImg->data + p);
        RASTERON_DEALLOC(tempImg);
    }
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

int main(int argc, char** argv){
    pixelPointTable.pointCount = 0;
    colorPointTable.pointCount = 0;

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = solidImgOp((ImageSize){1024, 1024}, 0xFF333333); // mapImgOp((ImageSize){1024, 1024}, waveDraw); // global canvas for drawing

    _run(argc, argv); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}