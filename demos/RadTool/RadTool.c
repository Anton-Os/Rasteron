#include "../_Catalouge.h"

#define RADIAL_SEGS 10.0
#define RADIAL_DIST 10.0
#define RADIAL_INVOKE 1024

#include "../_Demo.h"

double segs = RADIAL_SEGS;
double dist = RADIAL_DIST;
unsigned color1 = 0xFF111188;
unsigned color2 = 0xFF88EEEE;

static char keysave = '0';

#include "Rad.c"

// Overriden Functions

void _onKeyEvent(char key){
    if(isalpha(key)) RASTERON_DEALLOC(_outputImg);

    color1 = RAND_COLOR(); color2 = RAND_COLOR();
    unsigned radialColors[4] = { color1, color2, color_invert(color1), color_invert(color2) };
    switch(tolower(key)){
        case 'q': _outputImg = spiralsImgOp(0.1F * RADIAL_SEGS, 0.1F * RADIAL_DIST); break;
        case 'w': _outputImg = spiralsImgOp(0.1F * RADIAL_SEGS, 0.5F * RADIAL_DIST); break;
        case 'e': _outputImg = spiralsImgOp(0.5F * RADIAL_SEGS, 0.1F * RADIAL_DIST); break;
        case 'r': _outputImg = spiralsImgOp(1.0F * RADIAL_SEGS, 1.0F * RADIAL_DIST); break;
        case 't': _outputImg = spiralsImgOp(2.0F * RADIAL_SEGS, 2.0F * RADIAL_DIST); break;
        case 'y': _outputImg = spiralsImgOp(3.0F * RADIAL_SEGS, 3.0F * RADIAL_DIST); break;
        case 'u': _outputImg = spiralsImgOp(5.0F * RADIAL_SEGS, 5.0F * RADIAL_DIST); break;
        case 'i': _outputImg = spiralsImgOp(10.0F * RADIAL_SEGS, 10.0F * RADIAL_DIST); break;
        case 'o': _outputImg = spiralsImgOp(10.0F * RADIAL_SEGS, 100.0F * RADIAL_DIST); break;
        case 'p': _outputImg = spiralsImgOp(100.0F * RADIAL_SEGS, 10.0F * RADIAL_DIST); break;
        case 'a': _outputImg = radialImgOp(radialColors, radialMix1); break;
        case 's': _outputImg = radialImgOp(radialColors, radialMix2); break;
        case 'd': _outputImg = radialImgOp(radialColors, radialMix3); break;
        case 'f': _outputImg = radialImgOp(radialColors, radialMix4); break;
        case 'g': _outputImg = radialImgOp(radialColors, radialMix5); break;
        case 'h': _outputImg = radialImgOp(radialColors, radialMix6); break;
        case 'j': _outputImg = radialImgOp(radialColors, radialMix7); break;
        case 'k': _outputImg = radialImgOp(radialColors, radialMix8); break;
        case 'l': _outputImg = radialImgOp(radialColors, radialMix9); break;
        case 'z': _outputImg = mandalaImgOp(mandalaMap, mandalaMix1, mandalaInterp1); break;
        case 'x': _outputImg = mandalaImgOp(mandalaMap, mandalaMix2, mandalaInterp2); break;
        case 'c': _outputImg = mandalaImgOp(mandalaMap, mandalaMix3, mandalaInterp3); break;
        case 'v': _outputImg = mandalaImgOp(mandalaMap, mandalaMix4, mandalaInterp4); break;
        case 'b': _outputImg = mandalaImgOp(mandalaMap, mandalaMix5, mandalaInterp5); break;
        case 'n': _outputImg = mandalaImgOp(mandalaMap, mandalaMix6, mandalaInterp6); break;
        case 'm': _outputImg = mandalaImgOp(mandalaMap, mandalaMix7, mandalaInterp7); break;
        default: segs = 0.0; dist = 0.0; break;
    }
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

// Generative Function

Rasteron_Image* radTool(int argc, char** argv){
    return mandalaImgOp(mandalaMap, mandalaMix1, mandalaInterp7)
}

// Executable Function

int main(int argc, char** argv) {
	srand(time(NULL));

    _run(argc, argv, radTool); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
