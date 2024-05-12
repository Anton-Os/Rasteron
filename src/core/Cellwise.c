#include "Rasteron.h"

// Cellular operations

static Rasteron_Image* cellwiseImgOp_nebr8(ref_image_t refImage, nebrCallback8 callback){
	assert(refImage != NULL);

	NebrTable_List* nebrTables = loadNebrTables(refImage);
	Rasteron_Image* patternImg = RASTERON_ALLOC("pattern-nebr", refImage->height, refImage->width);

	for (unsigned p = 0; p < refImage->height * refImage->width; p++) {
		NebrTable* currentTable = nebrTables->tables + p;
		unsigned target = *(refImage->data + p);
		unsigned br, b, bl, r, l, tr, t, tl;

		unsigned short i = 0; // index to keep track of neighbor

		br = (currentTable->flags & (1 << NEBR_Bot_Right))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		b = (currentTable->flags & (1 << NEBR_Bot))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		bl = (currentTable->flags & (1 << NEBR_Bot_Left))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		r = (currentTable->flags & (1 << NEBR_Right))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		l = (currentTable->flags & (1 << NEBR_Left))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		tr = (currentTable->flags & (1 << NEBR_Top_Right))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		t = (currentTable->flags & (1 << NEBR_Top))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;
		tl = (currentTable->flags & (1 << NEBR_Top_Left))? *(*(currentTable->nebrs + (++i - 1))) : NO_COLOR;

		unsigned nebrs[] = { br, b, bl, r, l, tr, t, tl };

		unsigned newColor = callback(target, nebrs);
		if (newColor != NO_COLOR) *(patternImg->data + p) = newColor; // override color
		else *(patternImg->data + p) = *(refImage->data + p); // preserve color
	}

	delNebrTables(nebrTables);
	return patternImg;
}

Rasteron_Image* cellwiseRowImgOp(ref_image_t refImage, nebrCallback2 callback){
	assert(refImage != NULL);

	Rasteron_Image* stagingImg = RASTERON_ALLOC("staging", refImage->height, refImage->width);
		for (unsigned r = 0; r < refImage->width; r++)
			*(stagingImg->data + r) = *(refImage->data + r); // copy first row into staging image
	Rasteron_Image* patternImg = RASTERON_ALLOC("pattern-horz", refImage->height, refImage->width);

	for(unsigned r = 0; r < stagingImg->height; r++)
		for(unsigned c = 0; c < stagingImg->width; c++){
			unsigned p = (r * stagingImg->width) + c;

			unsigned target = *(stagingImg->data + p);
			unsigned right = (c < stagingImg->width - 1) ? *neighbor_get(stagingImg, p, NEBR_Right) : NO_COLOR;
			unsigned left = (c > 0) ? *neighbor_get(stagingImg, p, NEBR_Left) : NO_COLOR;

			unsigned nebrs[] = { right, left };

			unsigned newColor = callback(target, nebrs);
			if(newColor != NO_COLOR) *(patternImg->data + p) = newColor; // override color
			else *(patternImg->data + p) = *(stagingImg->data + p); // preserve color

			if(r < refImage->height - 1) // copy pixel to next row of staging image
				*(stagingImg->data + p + stagingImg->width) = *(patternImg->data + p);
		}

	RASTERON_DEALLOC(stagingImg);
	return patternImg;
}

Rasteron_Image* cellwiseColImgOp(ref_image_t refImage, nebrCallback2 callback){
	assert(refImage != NULL);

	Rasteron_Image* stagingImg = RASTERON_ALLOC("staging", refImage->height, refImage->width);
	for (unsigned c = 0; c < refImage->height; c++)
		*(stagingImg->data + (c * refImage->width)) = *(refImage->data + (c * refImage->width)); // copy first column
	Rasteron_Image* patternImg = RASTERON_ALLOC("pattern-vert", refImage->height, refImage->width);

	for (unsigned c = 0; c < stagingImg->width; c++) {
		for (unsigned r = 0; r < stagingImg->height; r++) {
			unsigned p = (r * stagingImg->width) + c;

			unsigned target = *(stagingImg->data + p);
			unsigned bot = (r < refImage->height - 1)? *neighbor_get(stagingImg, p, NEBR_Bot) : NO_COLOR;
			unsigned top = (r > 0) ? *neighbor_get(stagingImg, p, NEBR_Top) : NO_COLOR;

			unsigned nebrs[] = { bot, top };

			unsigned newColor = callback(target, nebrs);
			if (newColor != NO_COLOR) *(patternImg->data + p) = newColor; // override color
			else *(patternImg->data + p) = *(stagingImg->data + p); // preserve color

			if (c < refImage->width - 1) // copy pixel to next row of staging image
				*(stagingImg->data + p + 1) = *(patternImg->data + p);
		}
	}

	RASTERON_DEALLOC(stagingImg);
	return patternImg;
}


Rasteron_Image* cellwiseExtImgOp(ref_image_t refImage, nebrCallback8 callback, unsigned short iterations) {
	assert(refImage != NULL);
	if(iterations <= 0) return copyImgOp(refImage);
	
	Rasteron_Image* patternImage = copyImgOp(refImage);

	unsigned short i = 0;
	do {
		Rasteron_Image* stagingImg = cellwiseImgOp_nebr8(patternImage, callback);
		if(patternImage != NULL) RASTERON_DEALLOC(patternImage);
		patternImage = copyImgOp(stagingImg);
		RASTERON_DEALLOC(stagingImg);
		i++;
	} while (i < iterations);

	return patternImage;
}

unsigned antialias(unsigned target, unsigned neighbors[8]){
	unsigned long long finalColor = neighbors[0];
	unsigned short nCount = 1;

	for(unsigned n = 1; n < 8; n++)
		if(neighbors[n] != NO_COLOR)
			finalColor = colors_blend(finalColor, neighbors[n], 0.5F);
		else continue;

	/* for(unsigned n = 1; n < 8; n++){
		if(neighbors[n] != NO_COLOR){
			finalColor += neighbors[n];
			nCount++;
		}
	} */
	
	return finalColor / nCount;
}

Rasteron_Image* antialiasImgOp(ref_image_t refImage, unsigned short times){
	assert(refImage != NULL);
	if(times < 1) return copyImgOp(refImage);

	return cellwiseExtImgOp(refImage, antialias, times);
}


/* Rasteron_Image* stepImgOp(ref_image_t refImage, const PixelPointTable* pixelPointTable, stepCallback callback){
	if(refImage == NULL){
		perror("Cannot create step image! Null pointer provided as input");
		return NULL;
	}

	NebrTable_List* nebrTables = loadNebrTables(refImage);
	Rasteron_Image* stepImage = RASTERON_ALLOC("step", refImage->height, refImage->width);
	
	for(unsigned p = 0; p < stepImage->width * stepImage->height; p++)
		*(stepImage->data + p) = *(refImage->data + p); // copy pixel by pixel

	for(unsigned t = 0; t < pixelPointTable->pointCount; t++){
		unsigned offset = getPixOffset(pixelPointTable->points[t], stepImage);
		const NebrTable* currentTable = nebrTables->tables + offset;
		ColorStep colorStep = { *(stepImage->data + offset), NEBR_None };

		// for(unsigned s = 0; s < MAX_COLOR_STEPS && (colorStep.color != NO_COLOR && colorStep.direction != NEBR_None); s++){
		for(unsigned s = 0; s < MAX_COLOR_STEPS; s++){
			colorStep = callback(currentTable, colorStep, s);
			offset = neighbor_getOffset(refImage->width, offset, checkCanMove(currentTable->flags, colorStep.direction));
			currentTable = nebrTables->tables + offset;
			*(stepImage->data + offset) = colorStep.color;
		}
	}

	delNebrTables(nebrTables);
	return stepImage;
} */