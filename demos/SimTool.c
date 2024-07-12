#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define NSIM_COUNT 8

// ColorPointTable colorPointTable;

int mode = -1;

#include "Util_Demo.h"

static unsigned emptyColor = 0xFF333333;
static unsigned lifeColor = 0xFF00AA00;
static unsigned newColor =  0xFFAAFFAA;
static unsigned deadColor = 0xFFFF0000;
static unsigned oldColor = 0xFF111111;

static double killRate = 0.025; // 0.061;
static double feedRate = 0.025; // 0.025;

/* unsigned reactDiffuse(unsigned color, unsigned neighbors[8]){ 
    if(neighbors[0] == chemical2 || neighbors[1] == chemical2 || neighbors[2] == chemical2 || neighbors[3] == chemical2 || neighbors[4] == chemical2 || neighbors[5] == chemical2 || neighbors[6] == chemical2 || neighbors[7] == chemical2)
        return ((double)rand() / (double)RAND_MAX < feedRate)? chemical2 : color;
    if(neighbors[0] == chemical1 || neighbors[1] == chemical1 || neighbors[2] == chemical1 || neighbors[3] == chemical1 || neighbors[4] == chemical1 || neighbors[5] == chemical1 || neighbors[6] == chemical1 || neighbors[7] == chemical1)
        return ((double)rand() / (double)RAND_MAX < killRate)? 0xFF333333 : chemical1;
    return color; 
} */

void feedChems(Rasteron_Image* targetImg /* Rasteron_Image* lChemImg, Rasteron_Image* dChemImg */){
    for(unsigned p = 0; p < targetImg->width * targetImg->height; p++){
        if(*(targetImg->data + p) == deadColor)
            *(targetImg->data + p) = (rand() / RAND_MAX < killRate)? emptyColor : *(targetImg->data + p); // clears or stays same
        else if(*(targetImg->data + p) != lifeColor)
            *(targetImg->data + p) = (rand() / RAND_MAX < feedRate)? lifeColor : *(targetImg->data + p); // clears or stays same
    }
}

static unsigned conwayRules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(lifeColor, neighbors) + neighbor_count(newColor, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(deadColor, neighbors) + neighbor_count(oldColor, neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if((color == lifeColor || color == newColor) && lives < 2) return deadColor; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    else if((color == lifeColor || color == newColor) && (lives == 2 || lives == 3)) return lifeColor; // Any live cell with two or three live neighbors lives on to the next generation.
    else if((color == lifeColor || color == newColor) && lives > 3) return deadColor; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    else if((color == deadColor || color == oldColor) && lives == 3) return newColor; // Any kills cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    else if(color == deadColor) return oldColor;
    else return color; 
}

static unsigned boost(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(lifeColor, neighbors) + neighbor_count(newColor, neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(deadColor, neighbors) + neighbor_count(oldColor, neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if(lives > kills) return (color == lifeColor || color == newColor)? 0xFFEEEEEE : newColor; // color_level(color, 0.5 + (lives * (0.5 / 8.0)));
    else if(kills > lives) return (color == deadColor || color == oldColor)? 0xFFFF00FF : deadColor; // color_level(color, 0.5 - (lives * (0.5 / 8.0)));
    else return color;
}

Rasteron_Image* growImgOp(Rasteron_Image* refImg, double balance, double exFactor){
    Rasteron_Image* growthImg = copyImgOp(refImg);
    
    for(unsigned p = 0; p < growthImg->width * growthImg->height; p++){
        double prob = rand() / (double)RAND_MAX;

        unsigned color = newColor;
        if(prob > (1.0 - exFactor) * balance) color = newColor;
        else if(prob < exFactor * (1.0 / balance)) color = oldColor;
        else if(prob >= 0.5 * balance) color = lifeColor;
        else if(prob <= 0.5 * (1.0 / balance)) color = deadColor;
        else color = emptyColor;

        *(growthImg->data + p) = color;
    }
    
    return growthImg;
}

void populateQueue(Rasteron_Image* targetImg, nebrCallback8 callback){
    Rasteron_Image* simImg = copyImgOp(targetImg);

    for(unsigned f = 0; f < NSIM_COUNT; f++){
        if(simImg != NULL) RASTERON_DEALLOC(simImg);
        simImg = cellwiseExtImgOp(targetImg, callback, f + 1);
        Rasteron_Image* resizeImg = resizeImgOp((ImageSize){ 1024, 1024}, simImg);
        queue_addImg(_mainQueue, resizeImg, f);
        RASTERON_DEALLOC(resizeImg);
    }
}

void _onKeyEvent(char key){
    static unsigned short imageDiv = 1;
    static Rasteron_Image* backgroundImg = NULL; // solidImgOp((ImageSize){ 1024 / imageDiv, 1024 / imageDiv}, emptyColor);
    static Rasteron_Image* growImg = NULL; // growImgOp(backgroundImg, 1.0, 0.1);
    // static Rasteron_Image* simImg = cellwiseExtImgOp(growImg, conwayRules, 1);

    if(isspace(key)) mode *= -1; // switches mode into active state
    if(isdigit(key)){
        switch(key - '0'){
            case 0: imageDiv = 1; break;
            case 1: imageDiv = 2; break; case 2: imageDiv = 3; break; case 3: imageDiv = 4; break;
            case 4: imageDiv = 5; break; case 5: imageDiv = 6; break; case 6: imageDiv = 7; break;
            case 7: imageDiv = 8; break; case 8: imageDiv = 9; break; case 9: imageDiv = 10; break;
        }
    } else if(isalpha(key)){
        switch(key){
            // (q-p) // TODO: Change colors used
            // (a-l) // TODO: Change growImg initial conditions
            // (z-m) // TODO: Change simImg callback for model used
        }
    }
    else if(key == '\r'){
        puts("Return pressed, populate queue");
        if(growImg != NULL) populateQueue(growImg, conwayRules);
    }
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
    _outputImg = solidImgOp((ImageSize){ 1024, 1024 }, emptyColor);
    _mainQueue = RASTERON_QUEUE_ALLOC("sim", internal_create_size(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), NSIM_COUNT);

    populateQueue(_outputImg, conwayRules);
 
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    // _outputImg = resizeImgOp((ImageSize){ 1024, 1024}, simLifeImg);

    _run(argc, argv, NULL); // system specific initialization and continuous loop
    
    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    return 0;
}