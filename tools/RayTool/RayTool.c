#include "../_Tool.h"

// Overriden Functions

unsigned rayColor1 = 0xFF000000;
unsigned rayColor2 = 0xFFFFFFFF;

mixCallback vecMixFunc = mult_colors;
mixCallback rayMixFunc = bit_colors_xor;
coordCallback3 vectorFunc = NULL;

#include "Ray.c"

void _onKeyEvent(char key){
    float r = (((float)rand() / (float)RAND_MAX) - 0.5) * 2 * 2;
    float pointData[12] = { xArg, -yArg, (rand() % 2 == 0) ? r : -r, -xArg, yArg, r, xArg, yArg, -r, -xArg, -yArg, 0.0F };
    
    switch (key) {
        case 'q': vectorFunc = vectorFunc1; break;
        case 'w': vectorFunc = vectorFunc2; break;
        case 'e': vectorFunc = vectorFunc3; break;
        // TODO: Include other vector functions
        case 'a': rayMixFunc = bit_colors_xor; break;
        case 's': rayMixFunc = add_colors; break;
        case 'd': rayMixFunc = diff_colors; break;
    }
    if(key == ',') vecMixFunc = rayMixFunc;

    if (_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    if (key == 'q' || key == 'w' || key == 'e') _outputImg = vectorImgOp((ImageSize) { 1024, 1024 }, 1.0, vectorFunc);
    else _outputImg = raycastImgOp((ImageSize) { 1024, 1024 }, pointData, 12, 1.0);
}
void _onPressEvent(double x, double y){ }
void _onTickEvent(unsigned secs){}

int main(int argc, char** argv) {
    srand(time(NULL));
    
    rayColor1 = RAND_COLOR();
    rayColor2 = RAND_COLOR();

    vectorFunc = vectorFunc1;
    _outputImg = vectorImgOp((ImageSize){ 1024, 1024 }, 1.0, vectorFunc);

    _run(argc, argv, NULL); // system specific initialization and continuous loop

    RASTERON_DEALLOC(_outputImg); // cleanup
    return 0;
}
