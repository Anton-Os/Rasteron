#define NSIM_GROW 0.1
#define NSIM_COUNT 12

int mode = -1;

#include "_Demo.h"

#include "Sim.c"

// Overriden Functions

static void createSimSequence(Rasteron_Image* targetImg, nebrCallback8 algorithm, nebrCallback8 process){
    Rasteron_Image* simImg = NULL;

    for(unsigned f = 0; f < NSIM_COUNT; f++){
        if(simImg != NULL) RASTERON_DEALLOC(simImg);
        simImg = cellwiseExtImgOp(targetImg, algorithm, f + 1);
        Rasteron_Image* resizeImg = resizeImgOp((ImageSize){ 1024, 1024}, simImg);
        if(process == NULL) queue_addImg(_mainQueue, resizeImg, f);
        else {
            // TODO: Resize recursively!!!
            Rasteron_Image* processImg = cellwiseImgOp(resizeImg, process);
            queue_addImg(_mainQueue, processImg, f);
            RASTERON_DEALLOC(processImg);
        }
        RASTERON_DEALLOC(resizeImg);
    }
}

void _onKeyEvent(char key){
    static unsigned short div = 1;
    static double factor = 1.0;
    static Rasteron_Image* backgroundImg = NULL;
    static Rasteron_Image* growImg = NULL;
    static Rasteron_Image* stageImg = NULL;
    static nebrCallback8 algorithm = &conwayRules;
    static nebrCallback8 process = NULL;

    if(backgroundImg == NULL) backgroundImg = solidImgOp((ImageSize){ RASTERON_WIN_HEIGHT / _dimens[0], RASTERON_WIN_WIDTH / _dimens[1]}, _swatch.base);
    if(growImg == NULL) growImg = growImgOp(backgroundImg, 1.0, 0.1);

    if(isalpha(key)){
        if(backgroundImg != NULL) RASTERON_DEALLOC(backgroundImg);
        backgroundImg = solidImgOp((ImageSize){ RASTERON_WIN_HEIGHT / _dimens[0], RASTERON_WIN_WIDTH / _dimens[1]}, _swatch.base);
        switch(key){ case 'q': case 'w': case 'e': case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p': RASTERON_DEALLOC(growImg); break; } // delete old growth image
        switch(key){
            case 'q': growImg = growImgOp(backgroundImg, 1.0, 0.1); break;
            case 'w': growImg = growImgOp(backgroundImg, 1.0, 0.33); break;
            case 'e': growImg = feedImgOp(NULL, 3); break;
            case 'r': growImg = checkeredImgOp((ImageSize){ 1024 / _dimens[0], 1024 / _dimens[1] }, (ColorGrid){ 30 / _dimens[0], 30 / _dimens[1], _swatch.colors[SWATCH_Green_Add], _swatch.colors[SWATCH_Red_Add] }); break;
            case 't': growImg = linedImgOp((ImageSize){ 1024 / _dimens[0], 1024 / _dimens[1] }, _swatch.colors[SWATCH_Green_Add], _swatch.colors[SWATCH_Red_Add], 30 / _dimens[1], 0.0); break;
            case 'y': case 'u': case 'i': case 'o': 
                stageImg = seedImgOp(backgroundImg, NSIM_GROW); 
                if(key == 'y' || key == 'u') growImg = (key == 'y')? cellwiseColImgOp(stageImg, addlineRules) : cellwiseRowImgOp(stageImg, addlineRules); 
                else growImg = (key == 'i')? cellwiseColImgOp(stageImg, serpinskyRules) : cellwiseRowImgOp(stageImg, serpinskyRules);
                RASTERON_DEALLOC(stageImg);
            break; 
            case 'p': growImg = seedImgOp(backgroundImg, NSIM_GROW); break;
            case 'a': algorithm = &conwayRules; break;
            case 's': algorithm = &randWalkRules; break;
            case 'd': algorithm = &amplifyRules; break;
            case 'f': algorithm = &levelRules; break;
            case 'g': algorithm = &matchRules; break;
            case 'h': algorithm = &recursiveRules; break;
            case 'j': algorithm = &bloomRules; break;
            case 'k': algorithm = &unbloomRules; break;
            case 'l': algorithm = &wolframRules; break;
            case 'z': process = NULL; break;
            case 'x': process = algorithm; break;
            case 'c': process = antialias; break;
            case 'v': process = colorizeRules; break;
            case 'b': process = scatterRules; break;
            case 'n': process = recursiveRules; break;
            case 'm': process = flipRules; break;
        }

        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        _outputImg = resizeImgOp((ImageSize){ 1024, 1024}, growImg);
    }
    // else if(key == '\r'){
    else if(isspace(key)){
        createSimSequence(growImg, algorithm, process);
        mode *= -1; // indicating resume
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

// Generative Function

Rasteron_Image* simTool(int argc, char** argv){
    Rasteron_Image* backgroundImg = solidImgOp((ImageSize){ 1024, 1024 }, _swatch.base);
    Rasteron_Image* growthImg = growImgOp(backgroundImg, 0.5, 0.1);
    Rasteron_Image* simImg = simImgOp(growthImg, 1, conwayRules);

    RASTERON_DEALLOC(backgroundImg);
    RASTERON_DEALLOC(growthImg);

    return simImg;
}

// Executable Function

int main(int argc, char** argv) {
    _mainQueue = RASTERON_QUEUE_ALLOC("sim", internal_create_size(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), NSIM_COUNT);
 
    _run(argc, argv, simTool); // system specific initialization and continuous loop
    
    RASTERON_DEALLOC(_outputImg);
    RASTERON_QUEUE_DEALLOC(_mainQueue);

    return 0;
}
