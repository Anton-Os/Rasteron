#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define NSIM_COUNT 10

#define EMPTY_COLOR 0xFF333333
#define LIFE_COLOR 0xFF00FF00 // for cells surviving a while
#define NEWLIFE_COLOR 0xFFAAFFAA
#define KILL_COLOR 0xFFFF0000
#define OLDKILL_COLOR 0xFFAA0000

#include "Util_Demo.h"

static unsigned growth(unsigned color, unsigned neighbors[8]){ return EMPTY_COLOR; } // TODO: Implement this

Rasteron_Image* growLifeImgOp(unsigned short intensity, Rasteron_Image* image){ return copyImgOp(image); } // TODO: Implement this

static unsigned rules(unsigned color, unsigned neighbors[8]){ 
    // return NEWLIFE_COLOR; // Any dead cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    // return LIFE_COLOR // Any live cell with two or three live neighbors lives on to the next generation.
    // return KILL_COLOR; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    // return KILL_COLOR; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    // return OLDKILL_COLOR; // (if cell is still dead from last iteration)
    return EMPTY_COLOR; // TODO: Implement this
}

Rasteron_Image* simulateLifeImgOp(Rasteron_Image* image){ return copyImgOp(image); } // TODO: Implement this

static unsigned enhance(unsigned color, unsigned neighbors[8]){ return color; } // TODO: Implement this

Rasteron_Image* enhanceImgOp(Rasteron_Image* image){ return copyImgOp(image); } // TODO: Implement this

void keyEvent(char key){}
void mouseEvent(double x, double y){}
void timerEvent(unsigned secs){ } // WHY THIS DOESNT WORK?

int main(int argc, char** argv) {
    // _outputImg = solidImgOp() // TODO: Create noise image
    Rasteron_Image* growthLifeImg = growLifeImgOp(1, _outputImg);
    Rasteron_Image* simLifeImg = simulateLifeImgOp(1, _outputImg);
    Rasteron_Image* enhanceImg = enhanceImgOp(1, _outputImg);

    // _mainQueue = RASTERON_QUEUE_ALLOC("tex", createImgSize(1024, 1024), NSIM_COUNT);

    inputLoop(NULL);

    // RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    RASTERON_DEALLOC(growthLifeImg);
    RASTERON_DEALLOC(simLifeImg);
    RASTERON_DEALLOC(enhanceImg);

    return 0;
}