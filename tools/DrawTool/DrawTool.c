#include "DrawTool.h"


PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

#include "../_Tool.h"

// Overriden Functions

void setup(char input){ // double (*xMod)(double), double (*yMod)(double)){
    if (_savedImg != NULL) RASTERON_DEALLOC(_savedImg);

    switch(input){
    case 'q': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw1); break;
    case 'w': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw2); break;
    case 'e': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw3); break;
    case 'r': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw4); break;
    case 't': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw5); break;
    case 'y': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw6); break;
    case 'u': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw7); break;
    case 'i': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw8); break;
    case 'o': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw9); break;
    case 'p': _savedImg = fieldImgOp((ImageSize){1300, 1300}, &colorPointTable, dotDraw0); break;
    case 'a': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw1); break;
    case 's': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw2); break;
    case 'd': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw3); break;
    case 'f': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw4); break;
    case 'g': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw5); break;
    case 'h': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw6); break;
    case 'j': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw7); break;
    case 'k': _savedImg = brushDrawImgOp((ImageSize){1300, 1300}, brushDraw8); break;
    case 'z': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw1); break;
    case 'x': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw2); break;
    case 'c': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw3); break;
    case 'v': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw4); break;
    case 'b': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw5); break;
    case 'n': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw6); break;
    case 'm': _savedImg = fieldExtImgOp((ImageSize){ 1300, 1300 }, &colorPointTable, fieldDraw7); break;
    }

    if(_savedImg != NULL)
      for(unsigned p = 0; p < _savedImg->width * _savedImg->height; p++) 
        if(*(_savedImg->data + p) != NO_COLOR) *(_outputImg->data + p) = *(_savedImg->data + p);
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
    return checkeredImgOp((ImageSize){ 1300, 1300 }, (ColorGrid){ 10, 10, 0xFF333333, 0xFFEEEEEE });
}

// Executable Function

int main(int argc, char** argv){
    pixelPointTable.pointCount = 0;
    colorPointTable.pointCount = 0;

    _run(argc, argv, drawTool); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}