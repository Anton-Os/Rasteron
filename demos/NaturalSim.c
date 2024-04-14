#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define NSIM_COUNT 6

#define EMPTY_COLOR 0xFF333333
#define LIFE_COLOR 0xFF00FF00 // for cells surviving a while
#define NEWLIFE_COLOR 0xFFAAFFAA
#define KILL_COLOR 0xFFFF0000
#define OLDKILL_COLOR 0xFFAA0000

ColorPointTable colorPointTable;

#include "Util_Demo.h"

Rasteron_Image* growLifeImgOp(unsigned short intensity, Rasteron_Image* refImg){ 
    for(unsigned p = 0; p < MAX_PIXELPOINTS; p++){
        double prob = rand() / (double)RAND_MAX;

        unsigned color = NEWLIFE_COLOR;
        if(prob > 0.9) color = NEWLIFE_COLOR;
        else if(prob < 0.1) color = OLDKILL_COLOR;
        else if(prob > 0.5) color = LIFE_COLOR;
        else if(prob <= 0.5) color = KILL_COLOR;

        double xPos = rand() / (double)RAND_MAX;
        double yPos = rand() / (double)RAND_MAX;

        // colorPointToTable(&colorPointTable, color, pow(xPos, 5.0), pow(yPos, 5.0));
        // colorPointToTable(&colorPointTable, color, pow(xPos, (yPos > xPos)? 0.33 : 3.0), pow(yPos, (yPos > xPos < 1.0)? 0.33 : 3.0));
        colorPointToTable(&colorPointTable, color, pow(xPos, (yPos > xPos)? 0.33 : 3.0), pow(yPos, (yPos > xPos < 1.0)? 0.33 : 3.0));
    }
    return seededImgOp(refImg, &colorPointTable);
}

static unsigned short countLives(unsigned neighbors[8]){
    unsigned short lives = 0;
    for(unsigned n = 0; n < 8; n++) 
        if(neighbors[n] == LIFE_COLOR || neighbors[n] == NEWLIFE_COLOR || neighbors[n] == 0xFF00FFFF) lives++;
    return lives;
}

static unsigned short countKills(unsigned neighbors[8]){
    unsigned short kills = 0;
    for(unsigned n = 0; n < 8; n++) 
        if(neighbors[n] == KILL_COLOR || neighbors[n] == OLDKILL_COLOR || neighbors[n] ==  0xFFFF00FF) kills++;
    return kills;
}

static unsigned rules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = countLives(neighbors);
    unsigned short kills = countKills(neighbors);

    if((color == LIFE_COLOR || color == NEWLIFE_COLOR) && lives < 2) return KILL_COLOR; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    else if((color == LIFE_COLOR || color == NEWLIFE_COLOR) && (lives == 2 || lives == 3)) return LIFE_COLOR; // Any live cell with two or three live neighbors lives on to the next generation.
    else if((color == LIFE_COLOR || color == NEWLIFE_COLOR) && lives > 3) return KILL_COLOR; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    else if((color == KILL_COLOR || color == OLDKILL_COLOR) && lives == 3) return NEWLIFE_COLOR; // Any kills cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    else if(color == KILL_COLOR) return OLDKILL_COLOR;
    else return EMPTY_COLOR; 
}

static unsigned enhance(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = countLives(neighbors);
    unsigned short kills = countKills(neighbors);

    if(color == KILL_COLOR || color == OLDKILL_COLOR || color == LIFE_COLOR || color == NEWLIFE_COLOR) 
        return color; // neighbors[(lives + kills) % 8];
    else if(color == EMPTY_COLOR || color == 0xFF00FFFF || color == 0xFFFF00FF){
        if(lives > kills) return 0xFF00FFFF;
        else if(kills > lives) return 0xFFFF00FF;
        else return EMPTY_COLOR;
    }
    else return EMPTY_COLOR;
        // return colors_blend(colors_blend(neighbors[lives], neighbors[8 - lives - 1], 0.5), colors_blend(neighbors[kills], neighbors[8 - kills - 1], 0.5), 0.5);
    // else return EMPTY_COLOR; 
}


Rasteron_Image* simulateLifeImgOp(Rasteron_Image* refImg){ 
    for(unsigned i = 0; i < NSIM_COUNT; i++){
        Rasteron_Image* simImg = (i != 0)? cellwiseExtImgOp(refImg, rules, i) : copyImgOp(refImg);
        Rasteron_Image* largeImg = resizeImgOp((ImageSize){ 1024, 1024}, simImg);
        Rasteron_Image* enhanceImg = cellwiseExtImgOp(largeImg, enhance, 2);
        queue_addImg(_mainQueue, enhanceImg, i);

        RASTERON_DEALLOC(simImg);
        RASTERON_DEALLOC(largeImg);
        RASTERON_DEALLOC(enhanceImg);
    }

    return queue_getImg(_mainQueue, 0);
}

void _onKeyEvent(char key){}
void _onPressEvent(double x, double y){}
void _onTickEvent(unsigned secs){ 
    int index = secs % NSIM_COUNT;

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    Rasteron_Image* queueImg = queue_getImg(_mainQueue, index);
    _outputImg = copyImgOp(queueImg);
    RASTERON_DEALLOC(queueImg);
}

int main(int argc, char** argv) {
    _mainQueue = RASTERON_QUEUE_ALLOC("sim", createImgSize(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), NSIM_COUNT);

    Rasteron_Image* backgroundImg = solidImgOp((ImageSize){ 1024 / 5, 1024 / 5 }, EMPTY_COLOR); // TODO: Create noise refImg
    Rasteron_Image* growthLifeImg = growLifeImgOp(1, backgroundImg);
    Rasteron_Image* simLifeImg = simulateLifeImgOp(growthLifeImg);
    // Rasteron_Image* enhanceImg = enhanceImgOp(1, _outputImg);

    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = resizeImgOp((ImageSize){ 1024, 1024}, simLifeImg);

    // _mainQueue = RASTERON_QUEUE_ALLOC("tex", createImgSize(1024, 1024), NSIM_COUNT);

    _run();

    // RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    RASTERON_DEALLOC(backgroundImg);
    RASTERON_DEALLOC(growthLifeImg);
    RASTERON_DEALLOC(simLifeImg);
    // RASTERON_DEALLOC(enhanceImg);

    return 0;
}