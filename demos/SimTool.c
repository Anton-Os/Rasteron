#define RASTERON_ENABLE_ANIM

#include "Rasteron.h"

#define NSIM_COUNT 12

// ColorPointTable colorPointTable;

int mode = -1;

#include "Util_Demo.h"

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

/* unsigned reactDiffuse(unsigned color, unsigned neighbors[8]){ 
    if(neighbors[0] == chemical2 || neighbors[1] == chemical2 || neighbors[2] == chemical2 || neighbors[3] == chemical2 || neighbors[4] == chemical2 || neighbors[5] == chemical2 || neighbors[6] == chemical2 || neighbors[7] == chemical2)
        return ((double)rand() / (double)RAND_MAX < feedRate)? chemical2 : color;
    if(neighbors[0] == chemical1 || neighbors[1] == chemical1 || neighbors[2] == chemical1 || neighbors[3] == chemical1 || neighbors[4] == chemical1 || neighbors[5] == chemical1 || neighbors[6] == chemical1 || neighbors[7] == chemical1)
        return ((double)rand() / (double)RAND_MAX < killRate)? 0xFF333333 : chemical1;
    return color; 
} */

Rasteron_Image* feedChemOp(ref_image_t refImg, unsigned short iters){ // Rasteron_Image* lChemImg, Rasteron_Image* dChemImg){
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

unsigned antialias(unsigned target, unsigned neighbors[8]); // Defined in Cellwise.c

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

// static unsigned upscaleRules(unsigned color, unsigned neighbors[8]){}

Rasteron_Image* simImgOp(ref_image_t refImage){
    NebrTable_List* nebrTables = loadNebrTables(refImage);
	Rasteron_Image* cellwiseImg = RASTERON_ALLOC("cellwise-nebr", refImage->height, refImage->width);

	for (unsigned p = 0; p < refImage->height * refImage->width; p++) {
		NebrTable* currentTable = nebrTables->tables + p;
		unsigned* target = refImage->data + p;
		unsigned *br, *b, *bl, *r, *l, *tr, *t, *tl;

        neighbors_load(currentTable, br, b, bl, r, l, tr, t, tl);

        // TODO: Perform operations
	}

	delNebrTables(nebrTables);
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
            case 'e': growImg = feedChemOp(NULL, 3); break;
            case 'r': case 't': case 'y': case 'u': case 'i': case 'o': case 'p': 
                growImg = seedImgOp(backgroundImg, 0.1); break;
            case 'a': algorithm = &conwayRules; break;
            case 's': algorithm = &randWalkRules; break;
            case 'd': algorithm = &amplifyRules; break;
            case 'f': case 'g': case 'h': case 'j': case 'k': case 'l': puts("New algorithm selected"); break;
            case 'z': process = NULL; break;
            case 'x': process = algorithm; break;
            case 'c': process = antialias; break;
            case 'v': case 'b': case 'n': case 'm': puts("New processing selected"); break;
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
    _outputImg = growImgOp(backgroundImg, 0.5, 0.1);
    _mainQueue = RASTERON_QUEUE_ALLOC("sim", internal_create_size(RASTERON_WIN_HEIGHT, RASTERON_WIN_WIDTH), NSIM_COUNT);

    // fillSimQueue(_outputImg, conwayRules);
 

    _run(argc, argv, NULL); // system specific initialization and continuous loop
    
    if(_outputImg != NULL) RASTERON_DEALLOC(_outputImg);
    RASTERON_QUEUE_DEALLOC(_mainQueue);
    RASTERON_DEALLOC(_outputImg);

    RASTERON_DEALLOC(backgroundImg);

    return 0;
}