#include "Cellwise.h"

#include "stdio.h" // For debug purposes

static void setFlagBit(nebrCheckFlags* target, enum NBR_CellFlags flagBit){
    *(target) = (*(target) | (1 << (flagBit)));
    return;
}

static void clearFlagBit(nebrCheckFlags* target, enum NBR_CellFlags flagBit){
    *(target) = (*(target) & (~(1 << (flagBit))));
    return;
}

static nebrCheckFlags checkExistNebrs(uint32_t index, uint32_t width, uint32_t height){
    nebrCheckFlags flags = 0xFF;

    if(index < width){
        clearFlagBit(&flags, NBR_Top_Left);
        clearFlagBit(&flags, NBR_Top);
        clearFlagBit(&flags, NBR_Top_Right);
    } else if((width * height) - index - 1 < width){
        clearFlagBit(&flags, NBR_Bot_Left);
        clearFlagBit(&flags, NBR_Bot);
        clearFlagBit(&flags, NBR_Bot_Right);
    }

	if(index % width == 0){
        clearFlagBit(&flags, NBR_Top_Left);
        clearFlagBit(&flags, NBR_Left);
        clearFlagBit(&flags, NBR_Bot_Left);
    } else if(index % width == width - 1){
        clearFlagBit(&flags, NBR_Top_Right);
        clearFlagBit(&flags, NBR_Right);
        clearFlagBit(&flags, NBR_Bot_Right);
    }

    return flags;
}

static uint32_t* findNeighbor(Rasteron_Image* refImage, uint32_t index, enum NBR_CellFlags whichNebr){
    const uint32_t* target = refImage->data + index;

	switch (whichNebr) {
		case NBR_Bot_Right: return target + refImage->width + 1;
		case NBR_Bot: return target + refImage->width;
		case NBR_Bot_Left: return target + refImage->width - 1;
		case NBR_Right: return target + 1;
		case NBR_Left: return target - 1;
		case NBR_Top_Right: return target - refImage->width + 1;
		case NBR_Top: return target - refImage->width;
		case NBR_Top_Left: return target - refImage->width - 1;
	}
    return NULL;
}


// NebrTable_List* genNebrTables(const uint32_t* refImage->data, uint32_t refImage->width, uint32_t refImage->height) {
NebrTable_List* genNebrTables(const Rasteron_Image* refImage){
	NebrTable_List* list = (NebrTable_List*)malloc(sizeof(NebrTable_List));
	list->count = refImage->width * refImage->height;
	list->tables = (NebrTable*)malloc(refImage->width * refImage->height * sizeof(NebrTable));
	
	// const uint32_t* target = refImage->data; // target is a pointer to the first pixel
	unsigned pIndex = 0; // index to the pixel being processed
	for (NebrTable* currentTable = list->tables; // start with the first table
		currentTable != list->tables + (refImage->width * refImage->height); // pointer to end on in the mem space
		currentTable++) { // move to next table pointer

		currentTable->target = refImage->data + pIndex;
		currentTable->nebrExistFlags = checkExistNebrs(pIndex, refImage->width, refImage->height);

		unsigned short nebrCount = 0;
		for (unsigned short n = 0; n < 8; n++) // Determine number of neighbors based on nebrExistFlags
			if (currentTable->nebrExistFlags & (1 << (n))) // Traverse through all flags and increment if present
				nebrCount++;

		currentTable->nebrs = (uint32_t**)malloc(nebrCount * sizeof(uint32_t*));
		unsigned short i = 0; // index to keep track of neighbor
		for (unsigned short n = 0; n < 8; n++) {
			if (currentTable->nebrExistFlags & (1 << n)) {
				// unsigned* nebr = *(currentTable->nebrs + i);
				switch (n) {
				case NBR_Bot_Right: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Bot_Right); break;
				case NBR_Bot: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Bot); break;
				case NBR_Bot_Left: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Bot_Left); break;
				case NBR_Right: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Right); break;
				case NBR_Left : *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Left); break;
				case NBR_Top_Right: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Top_Right); break;
				case NBR_Top: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Top); break;
				case NBR_Top_Left: *(currentTable->nebrs + i) = findNeighbor(refImage, pIndex, NBR_Top_Left); break;
				}
				i++;
			}
		}

		pIndex++;
	}
	return list; // Return the structure that we generated
}

Rasteron_Image* createPatternImg4(const Rasteron_Image* refImage, fourNebrCallback callback){
	if(refImage == NULL){
		puts("Cannot create pattern image! Null pointer provided as input");
		return NULL;
	}

	NebrTable_List* nebrTables = genNebrTables(refImage);
	Rasteron_Image* fourNebrImg = allocNewImg("four-pattern", refImage->height, refImage->width);

	for(unsigned p = 0; p < refImage->height * refImage->width; p++){
		NebrTable* currentTable = nebrTables->tables + p;
		unsigned bottom, right, left, top;

		unsigned short i = 0; // index to keep track of neighbor
		if (currentTable->nebrExistFlags & (1 << NBR_Bot)) {
			bottom = *(currentTable->nebrs + i); i++;
		} else bottom = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Right)) {
			right = *(currentTable->nebrs + i); i++;
		} else right = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Left)) {
			left = *(currentTable->nebrs + i); i++;
		} else left = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Top)) {
			top = *(currentTable->nebrs + i); i++;
		} else top = BAD_COLOR_CODE;

		*(fourNebrImg->data + p) = callback(bottom, right, left, top);
	}

	delNebrTables(nebrTables);
	return fourNebrImg;
}

Rasteron_Image* createPatternImg8(const Rasteron_Image* refImage, eightNebrCallback callback){
	if(refImage == NULL){
		puts("Cannot create pattern image! Null pointer provided as input");
		return NULL;
	}

	NebrTable_List* nebrTables = genNebrTables(refImage);
	Rasteron_Image* eightNebrImg = allocNewImg("eight-pattern", refImage->height, refImage->width);

	for (unsigned p = 0; p < refImage->height * refImage->width; p++) {
		NebrTable* currentTable = nebrTables->tables + p;
		unsigned br, b, bl, r, l, tr, t, tl;

		unsigned short i = 0; // index to keep track of neighbor
		if (currentTable->nebrExistFlags & (1 << NBR_Bot_Right)) {
			br = *(currentTable->nebrs + i); i++;
		} else br = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Bot)) {
			b = *(currentTable->nebrs + i); i++;
		} else b = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Bot_Left)) {
			bl = *(currentTable->nebrs + i); i++;
		} else bl = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Right)) {
			r = *(currentTable->nebrs + i); i++;
		} else r = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Left)) {
			l = *(currentTable->nebrs + i); i++;
		} else l = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Top_Right)) {
			tr = *(currentTable->nebrs + i); i++;
		} else tr = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Top)) {
			t = *(currentTable->nebrs + i); i++;
		} else t = BAD_COLOR_CODE;
		if (currentTable->nebrExistFlags & (1 << NBR_Top_Left)) {
			tl = *(currentTable->nebrs + i); i++;
		} else tl = BAD_COLOR_CODE;

		*(eightNebrImg->data + p) = callback(br, b, bl, r, l, tr, t, tl);
	}

	delNebrTables(nebrTables);
	return eightNebrImg;
}

void delNebrTables(NebrTable_List* nebrTables) {
	for (NebrTable* currentTable = nebrTables->tables;
		currentTable != nebrTables->tables + nebrTables->count; 
		currentTable++) {
		free(currentTable->nebrs);
	}
	free(nebrTables->tables);
	free(nebrTables);
}