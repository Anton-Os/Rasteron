#include "RayTool.h"

unsigned rayColor1 = 0xFF000000;
unsigned rayColor2 = 0xFFFFFFFF;

coordCallback3 rayFunc = rayFunc1;
mixCallback rayMixFunc = bit_colors_xor;
mixCallback mixFunc = NULL;

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

#include "../_Tool.h"

void _onKeyEvent(char key){
    float r = (((float)rand() / (float)RAND_MAX) - 0.5) * 2;
    float pointData[POINTARRAY_COUNT] = { xArg, -yArg, (rand() % 2 == 0)? r : -r, -xArg, yArg, r, xArg, yArg, -r, -xArg, -yArg, 0.0F };
    if (pixelPointTable.pointCount > 0) { // filling points from 
        unsigned i = (pixelPointTable.pointCount > POINTARRAY_COUNT) ? pixelPointTable.pointCount - 12 : 0;
        for (unsigned p = 0; p < POINTARRAY_COUNT && p < pixelPointTable.pointCount; p++) {
            PixelPoint pixelPoint = pixelPointTable.points[i];
            pointData[p] = (p % 2 == 0)? pixelPoint.x : pixelPoint.y;
            i++;
        }
    }

    switch (key) {
        case 'q': rayFunc = rayFunc1; break;
        case 'w': rayFunc = rayFunc2; break;
        case 'e': rayFunc = rayFunc3; break;
        case 'r': rayFunc = rayFunc4; break;
        case 't': rayFunc = rayFunc5; break;
        case 'y': rayFunc = rayFunc6; break;
        case 'u': rayFunc = rayFunc7; break;
        case 'i': rayFunc = rayFunc8; break;
        case 'o': rayFunc = rayFunc9; break;
        case 'p': rayFunc = rayFunc10; break;
        case 'a': rayMixFunc = bit_colors_or; break;
        case 's': rayMixFunc = bit_colors_xor; break;
        case 'd': rayMixFunc = add_rgb; break;
        case 'f': rayMixFunc = add_colors; break;
        case 'g': rayMixFunc = diff_colors; break;
        case 'h': rayMixFunc = diff_rgb; break;
        case 'j': rayMixFunc = invertMix1; break;
        case 'k': rayMixFunc = invertMix2; break;
        case 'l': rayMixFunc = invertMix3; break;
        case 'z': mixFunc = mult_colors; break;
        case 'x': mixFunc = mult_rgb; break;
        case 'c': mixFunc = add_colors; break;
        case 'v': mixFunc = add_rgb; break;
        case 'b': mixFunc = bit_colors_and; break;
        case 'n': mixFunc = bit_colors_or; break;
        case 'm': mixFunc = sqroot_colors; break;
    }
    if (key == ',') {
        // mixFunc = rayMixFunc;
        rayColor1 = RAND_COLOR();
        rayColor2 = color_invert(rayColor1);
        // TODO: Change between rayMixFunc and mixFunc
    }

    if (_outputImg != NULL && isalpha(key)) RASTERON_DEALLOC(_outputImg);
    // if (KEYS_TOP_ROW(key)) _outputImg = vectorImgOp((ImageSize) { 1024, 1024 }, 1.0 * (mode + 1), rayFunc); 
    Rasteron_Image* raycastImg = raycastImgOp((ImageSize) { 1024, 1024 }, pointData, 12, abs((double)mode + 1));
    if (mixFunc != NULL && _savedImg != NULL)
        _outputImg = mixingImgOp(raycastImg, _savedImg, mixFunc);
    else _outputImg = copyImgOp(raycastImg);
    RASTERON_DEALLOC(raycastImg);
}
void _onPressEvent(double x, double y){ 
    pixelPointToTable(&pixelPointTable, x, y);
    colorPointToTable(&colorPointTable, color_level((colorPointTable.pointCount % 2 == 0) ? rayColor1 : rayColor2, (x + y) / 2.0), x, y);
}
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
    srand(time(NULL));
    
    rayColor1 = RAND_COLOR();
    rayColor2 = RAND_COLOR();

    rayFunc = rayFunc1;
    _outputImg = vectorImgOp((ImageSize){ 1024, 1024 }, 1.0, rayFunc);

    _run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
