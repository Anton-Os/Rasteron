#include "SimTool.h"

extern double killRate;
extern double feedRate; // 0.025;

extern unsigned _dimens[2];

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
