#include "Rasteron.h"

static unsigned callback_vornoi(unsigned color, double distance){ return color; }

Rasteron_Image* recolorImgOp(ref_image_t refImage, recolorCallback callback){
	assert(refImage != NULL);
    
    Rasteron_Image* recolorImage = alloc_image("recolor", refImage->height, refImage->width);

	for(unsigned p = 0; p < recolorImage->width * recolorImage->height; p++){
		unsigned newColor = callback(*(refImage->data + p));
		if(newColor != NO_COLOR) *(recolorImage->data + p) = newColor; // override color
		else *(recolorImage->data + p) = *(refImage->data + p); // preserve
	}

	return recolorImage;
}


Rasteron_Image* mapImgOp(ImageSize size, coordCallback callback){
	Rasteron_Image* mappedImage = alloc_image("map", size.height, size.width);

	for(unsigned p = 0; p < mappedImage->width * mappedImage->height; p++){
		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);
		*(mappedImage->data + p) = callback(x, y);
	}

	return mappedImage;
}

Rasteron_Image* fieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback callback) {
	Rasteron_Image* fieldImage = alloc_image("field", size.height, size.width);

	unsigned* colorPoints = malloc(colorPointTable->pointCount * sizeof(unsigned));
	for (unsigned t = 0; t < colorPointTable->pointCount; t++)
		*(colorPoints + t) = pixelPointOffset((PixelPoint){colorPointTable->points[t].x, colorPointTable->points[t].y}, fieldImage);
		// *(colorPoints + t) = pixelPointOffset({colorPointTable->points[t].x, colorPointTable->points[t].y}, fieldImage);

	for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
		unsigned color = BLACK_COLOR;
		double pixelSize = 1.0 / (double)(fieldImage->width); // fractional size of a pixel
		double minDist = 1.0;

		for (unsigned t = 0; t < colorPointTable->pointCount; t++) {
			double dist = pixelDistance(p, *(colorPoints + t), fieldImage->width) * pixelSize;
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

Rasteron_Image* vornoiImgOp(ImageSize size, const ColorPointTable* colorPointTable){
	return fieldImgOp(size, colorPointTable, callback_vornoi);
}

// Step operations

/* Rasteron_Image* stepImg(ref_image_t refImage, const PixelPointTable* pixelPointTable, stepCallback callback){
	if(refImage == NULL){
		perror("Cannot create step image! Null pointer provided as input");
		return NULL;
	}

	NebrTable_List* nebrTables = loadNebrTables(refImage);
	Rasteron_Image* stepImage = alloc_image("step", refImage->height, refImage->width);
	
	for(unsigned p = 0; p < stepImage->width * stepImage->height; p++)
		*(stepImage->data + p) = *(refImage->data + p); // copy pixel by pixel

	for(unsigned t = 0; t < pixelPointTable->pointCount; t++){
		unsigned offset = getPixOffset(pixelPointTable->points[t], stepImage);
		const NebrTable* currentTable = nebrTables->tables + offset;
		ColorStep colorStep = { *(stepImage->data + offset), NEBR_None };

		// for(unsigned s = 0; s < MAX_COLOR_STEPS && (colorStep.color != NO_COLOR && colorStep.direction != NEBR_None); s++){
		for(unsigned s = 0; s < MAX_COLOR_STEPS; s++){
			colorStep = callback(currentTable, colorStep, s);
			offset = findNeighborOffset(refImage->width, offset, checkCanMove(currentTable->flags, colorStep.direction));
			currentTable = nebrTables->tables + offset;
			*(stepImage->data + offset) = colorStep.color;
		}
	}

	delNebrTables(nebrTables);
	return stepImage;
} */