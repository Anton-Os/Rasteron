#include "Cellwise.h"

static void setFlagBit(nebrCheckFlags* target, enum NBR_CellFlags flagBit){
    *(target) = (*(target) | (1 << (flagBit)));
}

static void clearFlagBit(nebrCheckFlags* target, enum NBR_CellFlags flagBit){
    *(target) = (*(target) & (~(1 << (flagBit))));
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

static enum NBR_CellFlags checkNextMove(nebrCheckFlags flags, enum NBR_CellFlags direction){
	// TODO: Check for bad move
	return direction;
}

static unsigned findNeighborOffset(unsigned width, unsigned offset, enum NBR_CellFlags whichNebr){
	switch (whichNebr) {
		case NBR_Bot_Right: return offset + width + 1;
		case NBR_Bot: return offset + width;
		case NBR_Bot_Left: return offset + width - 1;
		case NBR_Right: return offset + 1;
		case NBR_Left: return offset - 1;
		case NBR_Top_Right: offset - width + 1;
		case NBR_Top: return offset - width;
		case NBR_Top_Left: return offset - width - 1;
		default: return offset;
	}
}

static uint32_t* findNeighbor(Rasteron_Image* refImage, uint32_t index, enum NBR_CellFlags whichNebr){
	const uint32_t* target = refImage->data + findNeighborOffset(refImage->width, index, whichNebr);
	return target;
}

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

void delNebrTables(NebrTable_List* nebrTables) {
	for (NebrTable* currentTable = nebrTables->tables;
		currentTable != nebrTables->tables + nebrTables->count; 
		currentTable++) {
		free(currentTable->nebrs);
	}
	free(nebrTables->tables);
	nebrTables->tables = NULL;
	free(nebrTables);
	nebrTables = NULL;
}

// Pattern operations

Rasteron_Image* createPatternImg(const Rasteron_Image* refImage, nebrCallback8 callback){
	if(refImage == NULL){
		perror("Cannot create pattern image! Null pointer provided as input");
		return NULL;
	}

	NebrTable_List* nebrTables = genNebrTables(refImage);
	Rasteron_Image* patternImg = allocNewImg("pattern-nebr", refImage->height, refImage->width);

	for (unsigned p = 0; p < refImage->height * refImage->width; p++) {
		NebrTable* currentTable = nebrTables->tables + p;
		unsigned target = *(refImage->data + p);
		unsigned br, b, bl, r, l, tr, t, tl;

		unsigned short i = 0; // index to keep track of neighbor
		if (currentTable->nebrExistFlags & (1 << NBR_Bot_Right)) {
			br = *(*(currentTable->nebrs + i)); i++;
		} else br = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Bot)) {
			b = *(*(currentTable->nebrs + i)); i++;
		} else b = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Bot_Left)) {
			bl = *(*(currentTable->nebrs + i)); i++;
		} else bl = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Right)) {
			r = *(*(currentTable->nebrs + i)); i++;
		} else r = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Left)) {
			l = *(*(currentTable->nebrs + i)); i++;
		} else l = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Top_Right)) {
			tr = *(*(currentTable->nebrs + i)); i++;
		} else tr = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Top)) {
			t = *(*(currentTable->nebrs + i)); i++;
		} else t = ZERO_COLOR;
		if (currentTable->nebrExistFlags & (1 << NBR_Top_Left)) {
			tl = *(*(currentTable->nebrs + i)); i++;
		} else tl = ZERO_COLOR;

		unsigned nebrs[] = { br, b, bl, r, l, tr, t, tl };

		unsigned newColor = callback(target, nebrs);
		if (newColor != ZERO_COLOR) *(patternImg->data + p) = newColor; // override color
		else *(patternImg->data + p) = *(refImage->data + p); // preserve color
	}

	delNebrTables(nebrTables);
	return patternImg;
}

Rasteron_Image* createMultiPatternImg(const Rasteron_Image* refImage, nebrCallback8 callback, unsigned short iter) {
	Rasteron_Image* patternImage = allocNewImg("staging", refImage->height, refImage->width);
	for (unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(patternImage->data + p) = *(refImage->data + p); // copy pixels from original image

	Rasteron_Image* stagingImg = { 0 };

	unsigned short i = 0;
	do {
		stagingImg = createPatternImg(patternImage, callback);
		for (unsigned p = 0; p < stagingImg->width * stagingImg->height; p++)
			*(patternImage->data + p) = *(stagingImg->data + p); // copy pixels from pattern image
		deleteImg(stagingImg);
		i++;
	} while (i < iter);

	return patternImage;
}

Rasteron_Image* createPatternImg_horz(const Rasteron_Image* refImage, nebrCallback2 callback){
	if(refImage == NULL){
		perror("Cannot create pattern image! Null pointer provided as input");
		return NULL;
	}

	Rasteron_Image* stagingImg = allocNewImg("staging", refImage->height, refImage->width);
		for (unsigned r = 0; r < refImage->width; r++)
			*(stagingImg->data + r) = *(refImage->data + r); // copy first row into staging image
	Rasteron_Image* patternImg = allocNewImg("pattern-horz", refImage->height, refImage->width);

	for(unsigned r = 0; r < stagingImg->height; r++)
		for(unsigned c = 0; c < stagingImg->width; c++){
			unsigned p = (r * stagingImg->width) + c;

			unsigned target = *(stagingImg->data + p);
			unsigned right = (c < stagingImg->width - 1) ? *findNeighbor(stagingImg, p, NBR_Right) : ZERO_COLOR;
			unsigned left = (c > 0) ? *findNeighbor(stagingImg, p, NBR_Left) : ZERO_COLOR;

			unsigned nebrs[] = { right, left };

			unsigned newColor = callback(target, nebrs);
			if(newColor != ZERO_COLOR) *(patternImg->data + p) = newColor; // override color
			else *(patternImg->data + p) = *(stagingImg->data + p); // preserve color

			if(r < refImage->height - 1) // copy pixel to next row of staging image
				*(stagingImg->data + p + stagingImg->width) = *(patternImg->data + p);
		}

	deleteImg(stagingImg);
	return patternImg;
}

Rasteron_Image* createPatternImg_vert(const Rasteron_Image* refImage, nebrCallback2 callback){
	if(refImage == NULL){
		perror("Cannot create pattern image! Null pointer provided as input");
		return NULL;
	}

	Rasteron_Image* stagingImg = allocNewImg("staging", refImage->height, refImage->width);
	for (unsigned c = 0; c < refImage->height; c++)
		*(stagingImg->data + (c * refImage->width)) = *(refImage->data + (c * refImage->width)); // copy first column
	Rasteron_Image* patternImg = allocNewImg("pattern-vert", refImage->height, refImage->width);

	for (unsigned c = 0; c < stagingImg->width; c++) {
		for (unsigned r = 0; r < stagingImg->height; r++) {
			unsigned p = (r * stagingImg->width) + c;

			unsigned target = *(stagingImg->data + p);
			unsigned bot = (r < refImage->height - 1)? *findNeighbor(stagingImg, p, NBR_Bot) : ZERO_COLOR;
			unsigned top = (r > 0) ? *findNeighbor(stagingImg, p, NBR_Top) : ZERO_COLOR;

			unsigned nebrs[] = { bot, top };

			unsigned newColor = callback(target, nebrs);
			if (newColor != ZERO_COLOR) *(patternImg->data + p) = newColor; // override color
			else *(patternImg->data + p) = *(stagingImg->data + p); // preserve color

			if (c < refImage->width - 1) // copy pixel to next row of staging image
				*(stagingImg->data + p + 1) = *(patternImg->data + p);
		}
	}

	deleteImg(stagingImg);
	return patternImg;
}

// Field operations

static unsigned callback_vornoi(unsigned color, double distance){
	return color; // return solid color
}

Rasteron_Image* createFieldImg(ImageSize size, const ColorPointTable* colorPointTable, distCallback callback) {
	Rasteron_Image* fieldImage = allocNewImg("field", size.height, size.width);

	unsigned* colorPoints = malloc(colorPointTable->pointCount * sizeof(unsigned));
	for (unsigned t = 0; t < colorPointTable->pointCount; t++)
		*(colorPoints + t) = getPixOffset(colorPointTable->points[t].point, fieldImage);

	for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
		unsigned color = BLACK_COLOR;
		double pixelSize = 1.0 / (double)(fieldImage->width); // fractional size of a pixel
		double minDist = 1.0;

		for (unsigned t = 0; t < colorPointTable->pointCount; t++) {
			double dist = getPixDist(p, *(colorPoints + t), fieldImage->width) * pixelSize;
			if (dist < minDist) {
				minDist = dist;
				color = colorPointTable->points[t].color;
			}
			*(fieldImage->data + p) = callback(color, minDist);
		}
	}

	free(colorPoints);
	return fieldImage;
}

Rasteron_Image* createFieldImg_vornoi(ImageSize size, const ColorPointTable* colorPointTable){
	return createFieldImg(size, colorPointTable, callback_vornoi);
}

// Step operations

Rasteron_Image* createStepImg(const Rasteron_Image* refImage, const PixelPointTable* pixelPointTable, stepCallback callback){
	if(refImage == NULL){
		perror("Cannot create step image! Null pointer provided as input");
		return NULL;
	}

	NebrTable_List* nebrTables = genNebrTables(refImage);
	Rasteron_Image* stepImage = allocNewImg("step", refImage->height, refImage->width);
	
	for(unsigned p = 0; p < stepImage->width * stepImage->height; p++)
		*(stepImage->data + p) = *(refImage->data + p); // copy pixel by pixel

	for(unsigned t = 0; t < pixelPointTable->pointCount; t++){
		unsigned offset = getPixOffset(pixelPointTable->points[t], stepImage);
		const NebrTable* currentTable = nebrTables->tables + offset;
		ColorStep colorStep = { *(stepImage->data + offset), NBR_None };

		for(unsigned s = 0; s < MAX_COLOR_STEPS && colorStep.color != ZERO_COLOR; s++){
			colorStep = callback(currentTable, colorStep, s);
			offset = findNeighborOffset(refImage->width, offset, checkNextMove(currentTable->nebrExistFlags, colorStep.direction));
			currentTable = nebrTables->tables + offset;
			*(stepImage->data + offset) = colorStep.color;
		}
	}

	delNebrTables(nebrTables);
	return stepImage;
}