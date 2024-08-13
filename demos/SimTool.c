#include "Catalouge.h"

#define NSIM_GROW 0.1
#define NSIM_COUNT 12

// ColorPointTable colorPointTable;

int mode = -1;

#include "Util_Runner.h"

static double killRate = 0.1;
static double feedRate = 0.025; // 0.025;

Rasteron_Image* seedImgOp(Rasteron_Image* refImg, double prob){
    Rasteron_Image* seedImg = (refImg != NULL)? copyImgOp(refImg) : solidImgOp((ImageSize){1024 / _dimens[0], 1024 / _dimens[1]}, _swatch.colors[SWATCH_Dark]);

    for(unsigned p = 0; p < seedImg->width * seedImg->height; p++)
        if(((float)rand() / (float)RAND_MAX < prob)) *(seedImg->data + p) = (rand() % 2 == 0)? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Red_Add];
        // else *(seedImg->data + p) = _swatch.colors[SWATCH_Dark];

    return seedImg;
}

Rasteron_Image* growImgOp(Rasteron_Image* refImg, double balance, double exFactor){
    Rasteron_Image* growthImg = copyImgOp(refImg);
    
    for(unsigned p = 0; p < growthImg->width * growthImg->height; p++){
        double prob = rand() / (double)RAND_MAX;

        unsigned color = _swatch.base;
        if(prob > (1.0 - exFactor) * balance) color = _swatch.colors[SWATCH_Light];
        else if(prob < exFactor * (1.0 / balance)) color = _swatch.colors[SWATCH_Dark];
        else if(prob >= 0.5 * balance) color = _swatch.colors[SWATCH_Green_Add];
        else if(prob <= 0.5 * (1.0 / balance)) color = _swatch.colors[SWATCH_Red_Add];
        else color = _swatch.base;

        *(growthImg->data + p) = color;
    }
    
    return growthImg;
}

// unsigned lineRules(unsigned color, unsigned neighbors[2]){ return (rand() % 2 == 0)? color : (rand() % 2 == 0)? neighbors[0] : neighbors[1]; }
unsigned lineRules(unsigned color, unsigned neighbors[2]){ return (neighbors[0] == neighbors[1])? neighbors[0] + neighbors[1] : color; }

Rasteron_Image* feedImgOp(ref_image_t refImg, unsigned short iters){ // Rasteron_Image* lChemImg, Rasteron_Image* dChemImg){
    Rasteron_Image* chemsImg = (refImg != NULL)? copyImgOp(refImg) : seedImgOp(NULL, 0.5);

    for(unsigned short i = 0; i < iters; i++)
        for(unsigned p = 0; p < chemsImg->width * chemsImg->height; p++){
            if(*(chemsImg->data + p) == _swatch.colors[SWATCH_Red_Add])
                *(chemsImg->data + p) = ((float)rand() / (float)RAND_MAX < killRate)? _swatch.colors[SWATCH_Dark] : *(chemsImg->data + p); // clears or stays same
            else if(*(chemsImg->data + p) != _swatch.colors[SWATCH_Green_Add])
                *(chemsImg->data + p) = ((float)rand() / (float)RAND_MAX < feedRate)? _swatch.colors[SWATCH_Green_Add] : *(chemsImg->data + p); // clears or stays same */
        }

    return chemsImg;
}

static unsigned conwayRules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(_swatch.colors[SWATCH_Green_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Light], neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(_swatch.colors[SWATCH_Red_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Dark], neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if((color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) && lives < 2) return _swatch.colors[SWATCH_Red_Add]; // Any live cell with fewer than two live neighbors dies, as if by underpopulation.
    else if((color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) && (lives == 2 || lives == 3)) return _swatch.colors[SWATCH_Green_Add]; // Any live cell with two or three live neighbors lives on to the next generation.
    else if((color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light]) && lives > 3) return _swatch.colors[SWATCH_Red_Add]; // Any live cell with more than three live neighbors dies, as if by overpopulation.
    else if((color == _swatch.colors[SWATCH_Red_Add] || color == _swatch.colors[SWATCH_Dark]) && lives == 3) return _swatch.colors[SWATCH_Light]; // Any kills cell with exactly three live neighbors becomes a live cell, as if by reproduction.
    else if(color == _swatch.colors[SWATCH_Red_Add]) return _swatch.colors[SWATCH_Dark];
    else return color; 
}

static unsigned randWalkRules(unsigned color, unsigned neighbors[8]){
    static unsigned short direction = 0;

    if(color == _swatch.colors[SWATCH_Light]) return _swatch.colors[SWATCH_Green_Add];
    else if(color == _swatch.colors[SWATCH_Red_Add]) return _swatch.colors[SWATCH_Dark];
    else if(neighbors[direction] == _swatch.colors[SWATCH_Light] || neighbors[direction] == _swatch.colors[SWATCH_Green_Add]){
        direction = rand() % 8; // randomize walk direction
        return _swatch.colors[SWATCH_Light];
    }
    else if(neighbors[direction] == _swatch.colors[SWATCH_Dark] || neighbors[direction] == _swatch.colors[SWATCH_Red_Add]){
        direction = rand() % 8; // randomize walk direction
        return _swatch.colors[SWATCH_Red_Add];
    }
    else return color;
}

static unsigned amplifyRules(unsigned color, unsigned neighbors[8]){ 
    unsigned short lives = neighbor_count(_swatch.colors[SWATCH_Green_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Light], neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(_swatch.colors[SWATCH_Red_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Dark], neighbors); // countKills(neighbors);
    // printf("Lives is %d, %d", lives, kills);

    if(lives > kills) return (color == _swatch.colors[SWATCH_Green_Add] || color == _swatch.colors[SWATCH_Light])? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Light]; // color_level(color, 0.5 + (lives * (0.5 / 8.0)));
    else if(kills > lives) return (color == _swatch.colors[SWATCH_Red_Add] || color == _swatch.colors[SWATCH_Dark])? _swatch.colors[SWATCH_Dark] : _swatch.colors[SWATCH_Red_Add]; // color_level(color, 0.5 - (lives * (0.5 / 8.0)));
    else return color;
}

static unsigned recursiveRules(unsigned color, unsigned neighbors[8]){
    color += (neighbors[NEBR_Left] > neighbors[NEBR_Right])? 16 : -16;
    color += (neighbors[NEBR_Top] > neighbors[NEBR_Bot])? 16 : -16;
    color += (neighbors[NEBR_Top_Left] + neighbors[NEBR_Top_Right] > neighbors[NEBR_Bot_Left] + neighbors[NEBR_Bot_Right])? 32 : -32;
    return color;
}

static unsigned bloomRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Bot] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Top] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Left] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Right] == _swatch.colors[SWATCH_Green_Add])
        return _swatch.colors[SWATCH_Red_Add];
    else if(neighbors[NEBR_Bot_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Right] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Bot_Right] == _swatch.colors[SWATCH_Red_Add])
        return _swatch.colors[SWATCH_Green_Add];
    else return color;
}

static unsigned unbloomRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Bot_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Right] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Top_Left] == _swatch.colors[SWATCH_Red_Add] || neighbors[NEBR_Bot_Right] == _swatch.colors[SWATCH_Red_Add])
        return _swatch.colors[SWATCH_Green_Add];
    else if(neighbors[NEBR_Bot] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Top] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Left] == _swatch.colors[SWATCH_Green_Add] || neighbors[NEBR_Right] == _swatch.colors[SWATCH_Green_Add])
        return _swatch.colors[SWATCH_Red_Add];
    else return color;
}

static unsigned levelRules(unsigned color, unsigned neighbors[8]){
    unsigned short lives = neighbor_count(_swatch.colors[SWATCH_Green_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Light], neighbors); // countLives(neighbors);
    unsigned short kills = neighbor_count(_swatch.colors[SWATCH_Red_Add], neighbors) + neighbor_count(_swatch.colors[SWATCH_Dark], neighbors);
    unsigned short diff = abs((short)lives - (short)kills);

    if(diff == 0) return color;
    else if(diff % 2 == 1) return (diff == 1)? _swatch.colors[SWATCH_Red_Add] : _swatch.colors[SWATCH_Light];
    else return (diff == 2)? _swatch.colors[SWATCH_Green_Add] : _swatch.colors[SWATCH_Dark];
}

static unsigned matchRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Left] == neighbors[NEBR_Right]) return neighbors[NEBR_Left];
    else if(neighbors[NEBR_Top] == neighbors[NEBR_Bot]) return neighbors[NEBR_Top];
    else if(neighbors[NEBR_Top_Left] == neighbors[NEBR_Bot_Right]) return neighbors[NEBR_Bot_Right];
    else if(neighbors[NEBR_Bot_Left] == neighbors[NEBR_Top_Right]) return neighbors[NEBR_Top_Right];
    else return neighbors[rand() % 8];
}

static unsigned colorizeRules(unsigned color, unsigned neighbors[8]){
    if(color == _swatch.colors[SWATCH_Light] || color == _swatch.colors[SWATCH_Green_Add]) return colors_blend(color, _swatch.colors[SWATCH_Green_Add], (float)rand() / RAND_MAX);
    else if(color == _swatch.colors[SWATCH_Dark] || color == _swatch.colors[SWATCH_Red_Add]) return colors_blend(color, _swatch.colors[SWATCH_Red_Add], (float)rand() / RAND_MAX);
    else return color;
}

static unsigned scatterRules(unsigned color, unsigned neighbors[8]){
    if(neighbors[NEBR_Left] == neighbors[NEBR_Right] && neighbors[NEBR_Top] == neighbors[NEBR_Bot]) return _swatch.colors[rand() % 8];
    else return color;
}

unsigned antialias(unsigned target, unsigned neighbors[8]); // Defined in Cellwise.c

Rasteron_Image* simImgOp(ref_image_t refImage, unsigned short iters, nebrCallback8 callback){
	Rasteron_Image* cellwiseImg = copyImgOp(refImage);

    for(unsigned i = 0; i < iters; i++){
        Rasteron_Image* tempImg = resizeImgOp((ImageSize){ 1024 / (iters - i), 1024 / (iters - i) }, cellwiseImg);
        NebrTable_List* nebrTables = loadNebrTables(tempImg);
        for (unsigned p = 0; p < tempImg->height * tempImg->width; p++) {
            NebrTable* currentTable = nebrTables->tables + p;
            unsigned* target = tempImg->data + p;
            unsigned *br = NULL, *b = NULL, *bl = NULL, *r = NULL, *l = NULL, *tr = NULL, *t = NULL, *tl = NULL;

            neighbors_load(currentTable, br, b, bl, r, l, tr, t, tl);
            unsigned nebrs[] = { br, b, bl, r, l, tr, t, tl };

            unsigned color = callback(target, nebrs);
            if(color != NO_COLOR) *(tempImg->data + p) = color;
        }
        RASTERON_DEALLOC(cellwiseImg);
        cellwiseImg = copyImgOp(tempImg);
        delNebrTables(nebrTables);
    }

	return cellwiseImg;
}


void fillSimQueue(Rasteron_Image* targetImg, nebrCallback8 algorithm, nebrCallback8 process){
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

    puts("Populating queue finished");
}

void _onKeyEvent(char key){
    static unsigned short div = 1;
    static double factor = 1.0;
    static Rasteron_Image* backgroundImg = NULL;
    static Rasteron_Image* growImg = NULL;
    static nebrCallback8 algorithm = &conwayRules;
    static nebrCallback8 process = NULL;

    if(backgroundImg == NULL) backgroundImg = solidImgOp((ImageSize){ RASTERON_WIN_HEIGHT / _dimens[0], RASTERON_WIN_WIDTH / _dimens[1]}, _swatch.base);
    if(growImg == NULL) growImg = growImgOp(backgroundImg, 1.0, 0.1);

    // _swatch.colors[SWATCH_Green_Add] = RAND_COLOR(); _swatch.colors[SWATCH_Light] = color_level(_swatch.colors[SWATCH_Green_Add], 0.85); // randomize color
    // _swatch.colors[SWATCH_Red_Add] = RAND_COLOR(); _swatch.colors[SWATCH_Dark] = color_level(_swatch.colors[SWATCH_Red_Add], 0.15); // randomize color

    printf("On key event with key %c", key);
    if(isspace(key)) mode *= -1; // switches mode into active state
    else if(isalpha(key)){
        if(backgroundImg != NULL) RASTERON_DEALLOC(backgroundImg);
        backgroundImg = solidImgOp((ImageSize){ RASTERON_WIN_HEIGHT / _dimens[0], RASTERON_WIN_WIDTH / _dimens[1]}, _swatch.base);
        switch(key){ case 'q': case 'w': case 'e': case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p': RASTERON_DEALLOC(growImg); break; } // delete old growth image
        switch(key){
            case 'q': growImg = growImgOp(backgroundImg, 1.0, 0.1); break;
            case 'w': growImg = growImgOp(backgroundImg, 1.0, 0.33); break;
            case 'e': growImg = feedImgOp(NULL, 3); break;
            case 'r': growImg = checkeredImgOp((ImageSize){ 1024 / _dimens[0], 1024 / _dimens[1] }, (ColorGrid){ 30 / _dimens[0], 30 / _dimens[1], _swatch.colors[SWATCH_Green_Add], _swatch.colors[SWATCH_Red_Add] }); break;
            case 't': growImg = linedImgOp((ImageSize){ 1024 / _dimens[0], 1024 / _dimens[1] }, _swatch.colors[SWATCH_Green_Add], _swatch.colors[SWATCH_Red_Add], 30 / _dimens[1], 0.0); break;
            case 'y': case 'u':
                Rasteron_Image* tempImg = seedImgOp(backgroundImg, NSIM_GROW); 
                growImg = (key == 'y')? cellwiseColImgOp(tempImg, lineRules) : cellwiseRowImgOp(tempImg, lineRules);
                RASTERON_DEALLOC(tempImg);
                break; 
            case 'i': case 'o': case 'p': growImg = seedImgOp(backgroundImg, (key == 'i')? NSIM_GROW : (key == 'o')? NSIM_GROW * 0.5 : NSIM_GROW * 0.1); break;
            case 'a': algorithm = &conwayRules; break;
            case 's': algorithm = &randWalkRules; break;
            case 'd': algorithm = &amplifyRules; break;
            case 'f': algorithm = &levelRules; break;
            case 'g': algorithm = &matchRules; break;
            case 'h': algorithm = &recursiveRules; break;
            case 'j': algorithm = &bloomRules; break;
            case 'k': algorithm = &unbloomRules; break;
            case 'l': puts("New algorithm selected"); break;
            case 'z': process = NULL; break;
            case 'x': process = algorithm; break;
            case 'c': process = antialias; break;
            case 'v': process = colorizeRules; break;
            case 'b': process = scatterRules; break;
            case 'n': process = recursiveRules; break;
            case 'm': puts("New processing selected"); break;
        }

        if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
        _outputImg = resizeImgOp((ImageSize){ 1024, 1024}, growImg);
    }
    // else if(key == '\r'){
    else if(key == '.'){
        mode *= -1; // indicating resume
        fillSimQueue(growImg, algorithm, process);
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
    Rasteron_Image* backgroundImg = solidImgOp((ImageSize){ 1024, 1024 }, _swatch.base);
    Rasteron_Image* growthImg = growImgOp(backgroundImg, 0.5, 0.1);
    _outputImg = simImgOp(growthImg, 1, conwayRules);
    _mainQueue = RASTERON_QUEUE_ALLOC("sim", internal_create_size(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), NSIM_COUNT);
 

    _run(argc, argv, NULL); // system specific initialization and continuous loop
    
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    RASTERON_DEALLOC(backgroundImg);

    return 0;
}