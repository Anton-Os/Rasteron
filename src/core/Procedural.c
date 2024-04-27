#include "Rasteron.h"

static unsigned callback_vornoi(unsigned color, double distance){ return color; }

Rasteron_Image* recolorImgOp(ref_image_t refImage, recolorCallback callback){
	assert(refImage != NULL);
    
    Rasteron_Image* recolorImage = RASTERON_ALLOC("recolor", refImage->height, refImage->width);

	for(unsigned p = 0; p < recolorImage->width * recolorImage->height; p++){
		unsigned newColor = callback(*(refImage->data + p));
		if(newColor != NO_COLOR) *(recolorImage->data + p) = newColor; // override color
		else *(recolorImage->data + p) = *(refImage->data + p); // preserve
	}

	return recolorImage;
}


Rasteron_Image* mapImgOp(ImageSize size, coordCallback callback){
	Rasteron_Image* mappedImage = RASTERON_ALLOC("map", size.height, size.width);

	for(unsigned p = 0; p < mappedImage->width * mappedImage->height; p++){
		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);
		*(mappedImage->data + p) = callback(x, y);
	}

	return mappedImage;
}

Rasteron_Image* fieldImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback callback) {
	Rasteron_Image* fieldImage = RASTERON_ALLOC("field", size.height, size.width);

	unsigned* colorPoints = malloc(colorPointTable->pointCount * sizeof(unsigned));
	for (unsigned t = 0; t < colorPointTable->pointCount; t++)
		*(colorPoints + t) = pixPoint_offset((PixelPoint){colorPointTable->points[t].x, colorPointTable->points[t].y}, fieldImage);
		// *(colorPoints + t) = pixPoint_offset({colorPointTable->points[t].x, colorPointTable->points[t].y}, fieldImage);

	for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
		unsigned color = BLACK_COLOR;
		double pixelSize = 1.0 / (double)(fieldImage->width); // fractional size of a pixel
		double minDist = 1.0;

		for (unsigned t = 0; t < colorPointTable->pointCount; t++) {
			double dist = pix_dist(p, *(colorPoints + t), fieldImage->width) * pixelSize;
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

Rasteron_Image* fieldExtImgOp(ImageSize size, const ColorPointTable* colorPointTable, fieldCallback3 callback) {
	Rasteron_Image* fieldImage = RASTERON_ALLOC("field", size.height, size.width);

	unsigned* colorPoints = malloc(colorPointTable->pointCount * sizeof(unsigned));
	for (unsigned t = 0; t < colorPointTable->pointCount; t++)
		*(colorPoints + t) = pixPoint_offset((PixelPoint){colorPointTable->points[t].x, colorPointTable->points[t].y}, fieldImage);
		// *(colorPoints + t) = pixPoint_offset({colorPointTable->points[t].x, colorPointTable->points[t].y}, fieldImage);

	double pixelSize = 1.0 / (double)(fieldImage->width); // fractional size of a pixel
	double pixColors[3] = { NO_COLOR, NO_COLOR, NO_COLOR };
	double pixDistances[3] = { 1.0, 1.0, 1.0 };

	for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
		for(unsigned t = 0; t < colorPointTable->pointCount; t++){
			double dist = pix_dist(p, *(colorPoints + t), fieldImage->width) * pixelSize;
			if(dist < pixDistances[0]){
				pixDistances[0] = dist;
				pixColors[0] = colorPointTable->points[t].color;
			} else if(dist < pixDistances[1]){
				pixDistances[1] = dist;
				pixColors[1] = colorPointTable->points[t].color;
			} else if(dist < pixDistances[2]){
				pixDistances[2] = dist;
				pixColors[2] = colorPointTable->points[t].color;
			}
		}

		*(fieldImage->data + p) = callback(pixColors, pixDistances);
	}

	free(colorPoints);
	return fieldImage;
}

Rasteron_Image* vornoiImgOp(ImageSize size, const ColorPointTable* colorPointTable){
	return fieldImgOp(size, colorPointTable, callback_vornoi);
}

/* Rasteron_Image* seededImgOp(ref_image_t refImage, const ColorPointTable* colorPointTable){
	assert(refImage != NULL);

	Rasteron_Image* seedImage = copyImgOp(refImage);

	for(unsigned p = 0; p < colorPointTable->pointCount; p++){
		ColorPoint colorPoint = colorPointTable->points[p];
		*(seedImage->data + pixPoint_offset((PixelPoint){ colorPoint.x, colorPoint.y }, refImage)) = colorPoint.color;
	}

	return seedImage;
} */

Rasteron_Image* checkerImgOp(ImageSize size, ColorGrid grid){
// Rasteron_Image* checkerImgOp(ImageSize size, Rasteron_Image* tileImages[2]){ // TODO: Replace with this
	Rasteron_Image* checkerImg = RASTERON_ALLOC("checker", size.height, size.width);

	for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

        unsigned c = x * grid.xCells;
        unsigned r = y * grid.yCells;

        if((c % 2 == 0 && r % 2 == 0) || (c % 2 == 1 && r % 2 == 1))
			*(checkerImg->data + p) = grid.color1;
        else *(checkerImg->data + p) = grid.color2; 
    }

	// TODO: Change this to enable Truchet tiling

	return checkerImg;
}

Rasteron_Image* gradientImgOp(ImageSize size, enum SIDE_Type side, unsigned color1, unsigned color2){
	if(side == SIDE_None) return solidImgOp(size, color1);

	Rasteron_Image* gradientImg = RASTERON_ALLOC("gradient", size.height, size.width);

	for(unsigned p = 0; p < size.width * size.height; p++){
		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

		if(side == SIDE_Left) *(gradientImg->data + p) = colors_fuse(color1, color2, x);
		else if(side == SIDE_Right) *(gradientImg->data + p) = colors_fuse(color1, color2, 1.0 - x);
		if(side == SIDE_Top) *(gradientImg->data + p) = colors_fuse(color1, color2, y);
		else if(side == SIDE_Bottom) *(gradientImg->data + p) = colors_fuse(color1, color2, 1.0 - y);
		else {
			double centerDist = pix_dist(p, ((size.width * size.height) / 2) + (size.width / 2), size.width) * (1.0 / (double)size.width);
			*(gradientImg->data + p) = colors_fuse(color1, color2, centerDist);
		}
	}


	return gradientImg;
}