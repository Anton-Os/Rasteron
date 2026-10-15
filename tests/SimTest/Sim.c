#include "SimTest.h"

extern double killRate;
extern double feedRate; // 0.025;

extern unsigned _dimens[2];

// Image Rules

unsigned addlineRules(unsigned cell, unsigned nerbs[2]) { return (nerbs[0] == nerbs[1]) ? nerbs[0] + nerbs[1] : cell; }

unsigned levelineRules(unsigned cell, unsigned nerbs[2]) { return (color_gray(cell) > color_gray(nerbs[1]))? color_level(cell, 0.33) : color_level(nerbs[0], 0.66); }

unsigned serpinskyRules(unsigned cell, unsigned nerbs[2]) {
    unsigned short s = 0;
    if (cell == SWATCH_GA || cell == SWATCH_L) s++;
    if (nerbs[0] == SWATCH_GA || nerbs[0] == SWATCH_L) s++;
    if (nerbs[1] == SWATCH_GA || nerbs[1] == SWATCH_L) s++;
    return (s == 1) ? SWATCH_GA : SWATCH_RA;
}

// Full-Cell Rules

unsigned shuffleRules(unsigned cell, unsigned nerbs[8]) {
    static unsigned r = 0;
    r++;
    for (unsigned n = 0; n < 8; n++) if(nerbs[n] == cell && nerbs[n] != NO_COLOR) return nerbs[r % 8];
    else return cell;
}

unsigned conwayRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs); // countKills(nerbs);
    // printf("Lives is %d, %d", lives, kills);

    if ((cell == SWATCH_GA || cell == SWATCH_L) && lives < 2) return SWATCH_RA; // Any live cell with fewer than two live nerbs dies, as if by underpopulation.
    else if ((cell == SWATCH_GA || cell == SWATCH_L) && (lives == 2 || lives == 3)) return SWATCH_GA; // Any live cell with two or three live nerbs lives on to the next generation.
    else if ((cell == SWATCH_GA || cell == SWATCH_L) && lives > 3) return SWATCH_RA; // Any live cell with more than three live nerbs dies, as if by overpopulation.
    else if ((cell == SWATCH_RA || cell == SWATCH_D) && lives == 3) return SWATCH_L; // Any kills cell with exactly three live nerbs becomes a live cell, as if by reproduction.
    else if (cell == SWATCH_RA) return SWATCH_D;
    else return cell;
}

unsigned randWalkRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short direction = 0;

    if (cell == SWATCH_L) return SWATCH_GA;
    else if (cell == SWATCH_RA) return SWATCH_D;
    else if (nerbs[direction] == SWATCH_L || nerbs[direction] == SWATCH_GA) {
        direction = rand() % 8; // randomize walk direction
        return SWATCH_L;
    }
    else if (nerbs[direction] == SWATCH_D || nerbs[direction] == SWATCH_RA) {
        direction = rand() % 8; // randomize walk direction
        return SWATCH_RA;
    }
    else return cell;
}

unsigned amplifyRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs); // countKills(nerbs);
    // printf("Lives is %d, %d", lives, kills);

    if (lives > kills) return (cell == SWATCH_GA || cell == SWATCH_L) ? SWATCH_GA : SWATCH_L; // color_level(cell, 0.5 + (lives * (0.5 / 8.0)));
    else if (kills > lives) return (cell == SWATCH_RA || cell == SWATCH_D) ? SWATCH_D : SWATCH_RA; // color_level(cell, 0.5 - (lives * (0.5 / 8.0)));
    else return cell;
}

unsigned recursiveRules(unsigned cell, unsigned nerbs[8]) {
    cell += (nerbs[NEBR_Left] > nerbs[NEBR_Right]) ? 16 : -16;
    cell += (nerbs[NEBR_Top] > nerbs[NEBR_Bot]) ? 16 : -16;
    cell += (nerbs[NEBR_Top_Left] + nerbs[NEBR_Top_Right] > nerbs[NEBR_Bot_Left] + nerbs[NEBR_Bot_Right]) ? 32 : -32;
    return cell;
}

unsigned bloomRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Bot] == SWATCH_GA || nerbs[NEBR_Top] == SWATCH_GA || nerbs[NEBR_Left] == SWATCH_GA || nerbs[NEBR_Right] == SWATCH_GA)
        return SWATCH_RA;
    else if (nerbs[NEBR_Bot_Left] == SWATCH_RA || nerbs[NEBR_Top_Right] == SWATCH_RA || nerbs[NEBR_Top_Left] == SWATCH_RA || nerbs[NEBR_Bot_Right] == SWATCH_RA)
        return SWATCH_GA;
    else return cell;
}

unsigned unbloomRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Bot_Left] == SWATCH_RA || nerbs[NEBR_Top_Right] == SWATCH_RA || nerbs[NEBR_Top_Left] == SWATCH_RA || nerbs[NEBR_Bot_Right] == SWATCH_RA)
        return SWATCH_GA;
    else if (nerbs[NEBR_Bot] == SWATCH_GA || nerbs[NEBR_Top] == SWATCH_GA || nerbs[NEBR_Left] == SWATCH_GA || nerbs[NEBR_Right] == SWATCH_GA)
        return SWATCH_RA;
    else return cell;
}

unsigned levelRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs);
    unsigned short diff = abs((short)lives - (short)kills);

    if (diff == 0) return cell;
    else if (diff % 2 == 1) return (diff == 1) ? SWATCH_RA : SWATCH_L;
    else return (diff == 2) ? SWATCH_GA : SWATCH_D;
}

unsigned matchRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Left] == nerbs[NEBR_Right]) return nerbs[NEBR_Left];
    else if (nerbs[NEBR_Top] == nerbs[NEBR_Bot]) return nerbs[NEBR_Top];
    else if (nerbs[NEBR_Top_Left] == nerbs[NEBR_Bot_Right]) return nerbs[NEBR_Bot_Right];
    else if (nerbs[NEBR_Bot_Left] == nerbs[NEBR_Top_Right]) return nerbs[NEBR_Top_Right];
    else return nerbs[rand() % 8];
}

unsigned colorizeRules(unsigned cell, unsigned nerbs[8]) {
    if (cell == SWATCH_L || cell == SWATCH_GA) return blend_colors(cell, SWATCH_GA, (float)rand() / RAND_MAX);
    else if (cell == SWATCH_D || cell == SWATCH_RA) return blend_colors(cell, SWATCH_RA, (float)rand() / RAND_MAX);
    else return cell;
}

unsigned scatterRules(unsigned cell, unsigned nerbs[8]) {
    if (nerbs[NEBR_Left] == nerbs[NEBR_Right] && nerbs[NEBR_Top] == nerbs[NEBR_Bot]) return _swatch.colors[rand() % 8];
    else return cell;
}

unsigned flipRules(unsigned cell, unsigned nerbs[8]) {
    if (cell == SWATCH_GA || cell == SWATCH_L) {
        unsigned lifeCount = 0;
        for (unsigned d = 0; d < 8; d++)
            if (nerbs[d] == SWATCH_GA || nerbs[d] == SWATCH_L) lifeCount++;
        return (lifeCount % 2 == 0) ? SWATCH_RA : SWATCH_D;
    }
    else if (cell == SWATCH_RA || cell == SWATCH_D) {
        unsigned deadCount = 0;
        for (unsigned d = 0; d < 8; d++)
            if (nerbs[d] == SWATCH_RA || nerbs[d] == SWATCH_D) deadCount++;
        return (deadCount % 2 == 0) ? SWATCH_GA : SWATCH_L;
    }
    return cell;
}

unsigned wolframRules(unsigned cell, unsigned nerbs[8]) {
    unsigned short lives = neighbor_count(SWATCH_GA, nerbs) + neighbor_count(SWATCH_L, nerbs); // countLives(nerbs);
    unsigned short kills = neighbor_count(SWATCH_RA, nerbs) + neighbor_count(SWATCH_D, nerbs);

    if (lives == 3 || lives == 5) return SWATCH_GA;
    if (kills == 2 || kills == 6) return SWATCH_RA;
    else return NO_COLOR;
}

// Image Operations

Rasteron_Image* seedImgOp(Rasteron_Image* refImg, double prob){
    Rasteron_Image* seedImg = (refImg != NULL)? copyImgOp(refImg) : solidImgOp((ImageSize){1300 / _dimens[0], 1300 / _dimens[1]}, SWATCH_D);

    for(unsigned p = 0; p < seedImg->width * seedImg->height; p++)
        if(((float)rand() / (float)RAND_MAX < prob)) *(seedImg->data + p) = (rand() % 2 == 0)? SWATCH_GA : SWATCH_RA;
        // else *(seedImg->data + p) = SWATCH_D;

    return seedImg;
}

Rasteron_Image* growImgOp(Rasteron_Image* refImg, double balance, double exFactor){
    Rasteron_Image* growthImg = copyImgOp(refImg);
    
    for(unsigned p = 0; p < growthImg->width * growthImg->height; p++){
        double prob = rand() / (double)RAND_MAX;

        unsigned color = _swatch.base;
        if(prob > (1.0 - exFactor) * balance) color = SWATCH_L;
        else if(prob < exFactor * (1.0 / balance)) color = SWATCH_D;
        else if(prob >= 0.5 * balance) color = SWATCH_GA;
        else if(prob <= 0.5 * (1.0 / balance)) color = SWATCH_RA;
        else color = _swatch.base;

        *(growthImg->data + p) = color;
    }
    
    return growthImg;
}

Rasteron_Image* feedImgOp(ref_image_t refImg, unsigned short iters){ // Rasteron_Image* lChemImg, Rasteron_Image* dChemImg){
    Rasteron_Image* chemsImg = (refImg != NULL)? copyImgOp(refImg) : seedImgOp(NULL, 0.5);

    for(unsigned short i = 0; i < iters; i++)
        for(unsigned p = 0; p < chemsImg->width * chemsImg->height; p++){
            if(*(chemsImg->data + p) == SWATCH_RA)
                *(chemsImg->data + p) = ((float)rand() / (float)RAND_MAX < killRate)? SWATCH_D : *(chemsImg->data + p); // clears or stays same
            else if(*(chemsImg->data + p) != SWATCH_GA)
                *(chemsImg->data + p) = ((float)rand() / (float)RAND_MAX < feedRate)? SWATCH_GA : *(chemsImg->data + p); // clears or stays same */
        }

    return chemsImg;
}

Rasteron_Image* simulationImgOp(ref_image_t refImage, unsigned short iters, nebrCallback8 callback){
	Rasteron_Image* cellwiseImg = copyImgOp(refImage);

    for(unsigned i = 0; i < iters; i++){
        Rasteron_Image* tempImg = resizeImgOp((ImageSize){ 1300 / (iters - i), 1300 / (iters - i) }, cellwiseImg);
        NebrTable_List* nebrTables = loadNebrTables(tempImg);
        for (unsigned p = 0; p < tempImg->height * tempImg->width; p++) {
            NebrTable* currentTable = nebrTables->tables + p;
            unsigned* target = tempImg->data + p;
            unsigned *br = NULL, *b = NULL, *bl = NULL, *r = NULL, *l = NULL, *tr = NULL, *t = NULL, *tl = NULL;

            neighbors_load(currentTable, br, b, bl, r, l, tr, t, tl);
            unsigned nebrs[] = { br, b, bl, r, l, tr, t, tl };
            // unsigned nebrs[] = { *br, *b, *bl, *r, *l, *tr, *t, *tl };

            // unsigned color = callback(target, nebrs);
            unsigned color = callback(*target, nebrs);
            if(color != NO_COLOR) *(tempImg->data + p) = color;
        }
        RASTERON_DEALLOC(cellwiseImg);
        cellwiseImg = copyImgOp(tempImg);
        delNebrTables(nebrTables);
    }

	return cellwiseImg;
}
