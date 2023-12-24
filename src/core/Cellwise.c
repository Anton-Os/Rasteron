#include "Rasteron.h"

// Cellular operations

static Rasteron_Image* cellswiseImgOp_nebr8(ref_image_t refImage, nebrCallback8 callback){
	assert(refImage != NULL);

	NebrTable_List* nebrTables = loadNebrTables(refImage);
	Rasteron_Image* patternImg = alloc_image("pattern-nebr", refImage->height, refImage->width);

	for (unsigned p = 0; p < refImage->height * refImage->width; p++) {
		NebrTable* currentTable = nebrTables->tables + p;
		unsigned target = *(refImage->data + p);
		unsigned br, b, bl, r, l, tr, t, tl;

		unsigned short i = 0; // index to keep track of neighbor
		if (currentTable->flags & (1 << NEBR_Bot_Right)) {
			br = *(*(currentTable->nebrs + i)); i++;
		} else br = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Bot)) {
			b = *(*(currentTable->nebrs + i)); i++;
		} else b = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Bot_Left)) {
			bl = *(*(currentTable->nebrs + i)); i++;
		} else bl = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Right)) {
			r = *(*(currentTable->nebrs + i)); i++;
		} else r = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Left)) {
			l = *(*(currentTable->nebrs + i)); i++;
		} else l = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Top_Right)) {
			tr = *(*(currentTable->nebrs + i)); i++;
		} else tr = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Top)) {
			t = *(*(currentTable->nebrs + i)); i++;
		} else t = NO_COLOR;
		if (currentTable->flags & (1 << NEBR_Top_Left)) {
			tl = *(*(currentTable->nebrs + i)); i++;
		} else tl = NO_COLOR;

		unsigned nebrs[] = { br, b, bl, r, l, tr, t, tl };

		unsigned newColor = callback(target, nebrs);
		if (newColor != NO_COLOR) *(patternImg->data + p) = newColor; // override color
		else *(patternImg->data + p) = *(refImage->data + p); // preserve color
	}

	delNebrTables(nebrTables);
	return patternImg;
}

Rasteron_Image* cellwiseImgOp2_horz(ref_image_t refImage, nebrCallback2 callback){
	assert(refImage != NULL);

	Rasteron_Image* stagingImg = alloc_image("staging", refImage->height, refImage->width);
		for (unsigned r = 0; r < refImage->width; r++)
			*(stagingImg->data + r) = *(refImage->data + r); // copy first row into staging image
	Rasteron_Image* patternImg = alloc_image("pattern-horz", refImage->height, refImage->width);

	for(unsigned r = 0; r < stagingImg->height; r++)
		for(unsigned c = 0; c < stagingImg->width; c++){
			unsigned p = (r * stagingImg->width) + c;

			unsigned target = *(stagingImg->data + p);
			unsigned right = (c < stagingImg->width - 1) ? *findNeighbor(stagingImg, p, NEBR_Right) : NO_COLOR;
			unsigned left = (c > 0) ? *findNeighbor(stagingImg, p, NEBR_Left) : NO_COLOR;

			unsigned nebrs[] = { right, left };

			unsigned newColor = callback(target, nebrs);
			if(newColor != NO_COLOR) *(patternImg->data + p) = newColor; // override color
			else *(patternImg->data + p) = *(stagingImg->data + p); // preserve color

			if(r < refImage->height - 1) // copy pixel to next row of staging image
				*(stagingImg->data + p + stagingImg->width) = *(patternImg->data + p);
		}

	dealloc_image(stagingImg);
	return patternImg;
}

Rasteron_Image* cellwiseImgOp2_vert(ref_image_t refImage, nebrCallback2 callback){
	assert(refImage != NULL);

	Rasteron_Image* stagingImg = alloc_image("staging", refImage->height, refImage->width);
	for (unsigned c = 0; c < refImage->height; c++)
		*(stagingImg->data + (c * refImage->width)) = *(refImage->data + (c * refImage->width)); // copy first column
	Rasteron_Image* patternImg = alloc_image("pattern-vert", refImage->height, refImage->width);

	for (unsigned c = 0; c < stagingImg->width; c++) {
		for (unsigned r = 0; r < stagingImg->height; r++) {
			unsigned p = (r * stagingImg->width) + c;

			unsigned target = *(stagingImg->data + p);
			unsigned bot = (r < refImage->height - 1)? *findNeighbor(stagingImg, p, NEBR_Bot) : NO_COLOR;
			unsigned top = (r > 0) ? *findNeighbor(stagingImg, p, NEBR_Top) : NO_COLOR;

			unsigned nebrs[] = { bot, top };

			unsigned newColor = callback(target, nebrs);
			if (newColor != NO_COLOR) *(patternImg->data + p) = newColor; // override color
			else *(patternImg->data + p) = *(stagingImg->data + p); // preserve color

			if (c < refImage->width - 1) // copy pixel to next row of staging image
				*(stagingImg->data + p + 1) = *(patternImg->data + p);
		}
	}

	dealloc_image(stagingImg);
	return patternImg;
}


Rasteron_Image* cellwiseImgOp(ref_image_t refImage, nebrCallback8 callback, unsigned short iterations) {
	assert(refImage != NULL);
	
	Rasteron_Image* patternImage = alloc_image("staging", refImage->height, refImage->width);
	for (unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(patternImage->data + p) = *(refImage->data + p); // copy pixels from original image

	Rasteron_Image* stagingImg = { 0 };

	unsigned short i = 0;
	do {
		stagingImg = cellswiseImgOp_nebr8(patternImage, callback);
		for (unsigned p = 0; p < stagingImg->width * stagingImg->height; p++)
			*(patternImage->data + p) = *(stagingImg->data + p); // copy pixels from pattern image
		dealloc_image(stagingImg);
		i++;
	} while (i < iterations);

	return patternImage;
}

unsigned antialias(unsigned target, unsigned neighbors[8]){
	unsigned finalColor = neighbors[0];

	for(unsigned n = 1; n < 8; n++)
		if(neighbors[n] != NO_COLOR)
			finalColor = blendColors(finalColor, neighbors[n], 0.5F);
		else continue;
	
	return finalColor;
}

Rasteron_Image* antialiasImgOp(ref_image_t refImage){
	assert(refImage != NULL);

	return cellwiseImgOp(refImage, antialias, 1);
}
