#include "../_Tool.h"

// Overriden Functions

unsigned rayColor1 = 0xFF000000;
unsigned rayColor2 = 0xFFFFFFFF;

mixCallback vecMixFunc = mult_colors;
mixCallback rayMixFunc = bit_colors_xor;
coordCallback3 rayFunc = NULL;

PixelPointTable pixelPointTable;
ColorPointTable colorPointTable;

#include "Ray.c"

// float pData[12] = { 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 0.0F };

void _onKeyEvent(char key){
    float r = (((float)rand() / (float)RAND_MAX) - 0.5) * 2 * 2;
    float pointData[12] = { xArg, -yArg, (rand() % 2 == 0)? r : -r, -xArg, yArg, r, xArg, yArg, -r, -xArg, -yArg, 0.0F };
    
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
        // TODO: Include other vector functions
        case 'a': rayMixFunc = bit_colors_or; break;
        case 's': rayMixFunc = bit_colors_xor; break;
        case 'd': rayMixFunc = add_rgb; break;
        case 'f': rayMixFunc = add_colors; break;
        case 'g': rayMixFunc = diff_colors; break;
        case 'h': rayMixFunc = diff_rgb; break;
        case 'j': rayMixFunc = invertMix1; break;
        case 'k': rayMixFunc = invertMix2; break;
        case 'l': rayMixFunc = invertMix3; break;
        // TODO: Include more variance functions
        case 'z': vecMixFunc = mult_colors; break;
        case 'x': vecMixFunc = mult_rgb; break;
        case 'c': vecMixFunc = add_colors; break;
        case 'v': vecMixFunc = add_rgb; break;
        case 'b': vecMixFunc = bit_colors_and; break;
        case 'n': vecMixFunc = bit_colors_or; break;
        case 'm': vecMixFunc = sqroot_colors; break;
    }
    if (key == ',') {
        // vecMixFunc = rayMixFunc;
        rayColor1 = RAND_COLOR();
        rayColor2 = color_invert(rayColor1);
    }

    if (_outputImg != NULL && isalpha(key)) RASTERON_DEALLOC(_outputImg);
    // if (KEYS_TOP_ROW(key)) _outputImg = vectorImgOp((ImageSize) { 1024, 1024 }, 1.0 * (mode + 1), rayFunc);
    // else if(KEYS_MID_ROW(key)) 
        _outputImg = raycastImgOp((ImageSize) { 1024, 1024 }, pointData, 12, abs((double)mode + 1));
    /* else {
        float* ptData = (float*)malloc(pixelPointTable.pointCount * sizeof(float) * 2);
        for (unsigned p = 0; p < pixelPointTable.pointCount; p++) {
            *(ptData + (p * 2)) = pixelPointTable.points[p].x;
            *(ptData + (p * 2) + 1) = pixelPointTable.points[p].y;
        }
        _outputImg = raycastImgOp((ImageSize) { 1024, 1024 }, ptData, pixelPointTable.pointCount, 1.0 * (mode + 1));
        free(ptData);
    } */
    // else raycastImgOp((ImageSize) { 1024, 1024 }, pixelPointTable.points, pixelPointTable.pointCount, 1.0 * (mode + 1));
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
