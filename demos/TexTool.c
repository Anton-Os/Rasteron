#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define TEXTOOL_COUNT 10

#include "Util_Demo.h"

ColorGrid grids[TEXTOOL_COUNT];

Rasteron_Image* texImages[TEXTOOL_COUNT];

void setupCanvas(int mode, Rasteron_Image* baseTex){
    if(mode >= 0 && mode <= TEXTOOL_COUNT){
        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        _outputImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, grids[mode].color1, grids[mode].color2);// copyImgOp(texImages[mode]);
    } 
}


void keyEvent(char key){ setupCanvas(key - '0', texImages[key - '0']); }
void mouseEvent(double x, double y){}
void timerEvent(unsigned secs){
    _mainQueue->index = secs % TEXTOOL_COUNT;
    // if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    // _outputImg = getFrameAt(_mainQueue, _mainQueue->index);
}

int main(int argc, char** argv) {
    // if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    // _outputImg = noiseImgOp_white((ImageSize){ 1024, 1024 }, 0xFF0000FF, 0xFFFF0000);

    // _outputImg = solidImgOp() // TODO: Create noise image
    grids[0] = (ColorGrid){ 2, 2, 0xFF333333, 0xFFEEEEEE }; texImages[0] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[0]);
    grids[1] = (ColorGrid){ 4, 4, 0xFF333333, 0xFFEEEEEE }; texImages[1] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[1]);
    grids[2] = (ColorGrid){ 8, 8, 0xFF333333, 0xFFEEEEEE }; texImages[2] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[2]);
    grids[3] = (ColorGrid){ 16, 16, 0xFF333333, 0xFFEEEEEE }; texImages[3] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[3]);
    grids[4] = (ColorGrid){ 32, 32, 0xFF333333, 0xFFEEEEEE }; texImages[4] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[4]);
    grids[5] = (ColorGrid){ 64, 64, 0xFF333333, 0xFFEEEEEE }; texImages[5] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[5]);
    grids[6] = (ColorGrid){ 128, 128, 0xFF333333, 0xFFEEEEEE }; texImages[6] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[6]);
    grids[7] = (ColorGrid){ 256, 256, 0xFF333333, 0xFFEEEEEE }; texImages[7] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[7]);
    grids[8] = (ColorGrid){ 512, 512, 0xFF333333, 0xFFEEEEEE }; texImages[8] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[8]);
    grids[9] = (ColorGrid){ 1024, 1024, 0xFF333333, 0xFFEEEEEE }; texImages[9] = noiseImgOp_value((ImageSize){ 1024, 1024 }, grids[9]);


    _mainQueue = RASTERON_QUEUE_ALLOC("tex", createImgSize(1024, 1024), TEXTOOL_COUNT);

    inputLoop(NULL);

    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    for(unsigned n = 0; n < TEXTOOL_COUNT; n++) RASTERON_DEALLOC(texImages[n]);

    return 0;
}