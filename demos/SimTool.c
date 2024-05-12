#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define NSIM_COUNT 8

#define EMPTY_COLOR 0xFF333333
#define LIFE_COLOR 0xFF00AA00 // for cells surviving a while
#define NEWLIFE_COLOR 0xFFAAFFAA
#define KILL_COLOR 0xFFFF0000
#define OLDKILL_COLOR 0xFF111111

// ColorPointTable colorPointTable;

int mode = -1;

#include "Util_Demo.h"

static double killRate = 0.1; // 0.061;
static unsigned chemical1 = 0xFFFF0000;
static double feedRate = 0.1; // 0.025;
static unsigned chemical2 = 0xFF0000FF;

/* unsigned reactDiffuse(unsigned color, unsigned neighbors[8]){ 
    if(neighbors[0] == chemical2 || neighbors[1] == chemical2 || neighbors[2] == chemical2 || neighbors[3] == chemical2 || neighbors[4] == chemical2 || neighbors[5] == chemical2 || neighbors[6] == chemical2 || neighbors[7] == chemical2)
        return ((double)rand() / (double)RAND_MAX < feedRate)? chemical2 : color;
    if(neighbors[0] == chemical1 || neighbors[1] == chemical1 || neighbors[2] == chemical1 || neighbors[3] == chemical1 || neighbors[4] == chemical1 || neighbors[5] == chemical1 || neighbors[6] == chemical1 || neighbors[7] == chemical1)
        return ((double)rand() / (double)RAND_MAX < killRate)? 0xFF333333 : chemical1;
    return color; 
} */

static unsigned conwayRules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(LIFE_COLOR, neighbors) + neighbor_count(NEWLIFE_COLOR, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(KILL_COLOR, neighbors) + neighbor_count(OLDKILL_COLOR, neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if((color == LIFE_COLOR || color == NEWLIFE_COLOR) && lives < 2) return KILL_COLOR; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    else if((color == LIFE_COLOR || color == NEWLIFE_COLOR) && (lives == 2 || lives == 3)) return LIFE_COLOR; // Any live cell with two or three live neighbors lives on to the next generation.
    else if((color == LIFE_COLOR || color == NEWLIFE_COLOR) && lives > 3) return KILL_COLOR; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    else if((color == KILL_COLOR || color == OLDKILL_COLOR) && lives == 3) return NEWLIFE_COLOR; // Any kills cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    else if(color == KILL_COLOR) return OLDKILL_COLOR;
    else return color; 
}

static unsigned boost(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(LIFE_COLOR, neighbors) + neighbor_count(NEWLIFE_COLOR, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(KILL_COLOR, neighbors) + neighbor_count(OLDKILL_COLOR, neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if(lives > kills) return (color == LIFE_COLOR || color == NEWLIFE_COLOR)? 0xFFEEEEEE : NEWLIFE_COLOR; // color_level(color, 0.5 + (lives * (0.5 / 8.0)));
    else if(kills > lives) return (color == KILL_COLOR || color == OLDKILL_COLOR)? 0xFFFF00FF : KILL_COLOR; // color_level(color, 0.5 - (lives * (0.5 / 8.0)));
    else return color;
}

Rasteron_Image* growImgOp(unsigned short intensity, Rasteron_Image* refImg){ 
    Rasteron_Image* growthImg = copyImgOp(refImg);
    
    for(unsigned p = 0; p < growthImg->width * growthImg->height; p++){
        double prob = rand() / (double)RAND_MAX;

        unsigned color = NEWLIFE_COLOR;
        if(prob > 0.9) color = NEWLIFE_COLOR;
        else if(prob < 0.1) color = OLDKILL_COLOR;
        else if(prob >= 0.5) color = LIFE_COLOR;
        else if(prob <= 0.5) color = KILL_COLOR;
        // else color = EMPTY_COLOR;

        *(growthImg->data + p) = color;
    }
    
    return growthImg;
}

/* Rasteron_Image* simImgOp(Rasteron_Image* refImg, unsigned short iters){ 
    Rasteron_Image* conwayImg = cellwiseExtImgOp(refImg, conwayRules, iters);
    Rasteron_Image* simImg = cellwiseExtImgOp(conwayImg, boost, iters);

    RASTERON_DEALLOC(conwayImg);

    return simImg;
} */

void _onKeyEvent(char key){
    if(isspace(key)) mode *= -1; // switches mode into active state
    // else if() // TODO: Set parameters
}

void _onPressEvent(double x, double y){}

void _onTickEvent(unsigned secs){ 
    if(mode > 0){
        int index = secs % (NSIM_COUNT - 1);
        
        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        Rasteron_Image* queueImg = queue_getImg(_mainQueue, index);
        _outputImg = copyImgOp(queueImg);
    }
    // RASTERON_DEALLOC(queueImg);
}

int main(int argc, char** argv) {
    _outputImg = solidImgOp((ImageSize){ 1024, 1024 }, EMPTY_COLOR);
    _mainQueue = RASTERON_QUEUE_ALLOC("sim", createImgSize(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), NSIM_COUNT);

    Rasteron_Image* backgroundImg = solidImgOp((ImageSize){ 1024 / 5, 1024 / 5}, EMPTY_COLOR); // TODO: Create noise refImg
    Rasteron_Image* growthLifeImg = growImgOp(1, backgroundImg);
    Rasteron_Image* simLifeImg = cellwiseExtImgOp(growthLifeImg, conwayRules, 1);
    for(unsigned f = 0; f < NSIM_COUNT; f++){
        if(simLifeImg != NULL) RASTERON_DEALLOC(simLifeImg);
        simLifeImg = cellwiseExtImgOp(growthLifeImg, conwayRules, f + 1);
        Rasteron_Image* resizeImg = resizeImgOp((ImageSize){ 1024, 1024}, simLifeImg);
        queue_addImg(_mainQueue, resizeImg, f);
        RASTERON_DEALLOC(resizeImg);
    }
 
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    _outputImg = resizeImgOp((ImageSize){ 1024, 1024}, simLifeImg);

    _run(argc, argv, NULL); // system specific initialization and continuous loop
    
    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);
    RASTERON_DEALLOC(backgroundImg);
    RASTERON_DEALLOC(growthLifeImg);
    // RASTERON_DEALLOC(simLifeImg);

    return 0;
}