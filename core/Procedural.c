#include "Rasteron.h"

static unsigned callback_vornoi(unsigned color, double distance, PixelPoint pixPoint){ return color; }

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

	for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
		unsigned color = NO_COLOR;
		double minDist = 1.0;
		PixelPoint pixPoint = (PixelPoint){ 0.0, 0.0 };

		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

		for (unsigned t = 0; t < colorPointTable->pointCount; t++) {
			double dist = pix_dist(p, *(colorPoints + t), fieldImage->width) * (1.0 / (double)(fieldImage->width)); // distance multiplied by pixel size
			if (dist < minDist) {
				minDist = dist;
				pixPoint = (PixelPoint){ x - colorPointTable->points[t].x, y - colorPointTable->points[t].y };
				color = colorPointTable->points[t].color;
			}
			*(fieldImage->data + p) = callback(color, minDist, pixPoint);
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

	unsigned pixColors[3] = { NO_COLOR, NO_COLOR, NO_COLOR };
	double pixDistances[3] = { 1.0, 1.0, 1.0 };
	PixelPoint pixPoints[3] = {{ 0.0, 0.0 }, { 0.0, 0.0 }, { 0.0, 0.0 }};

	for (unsigned p = 0; p < fieldImage->width * fieldImage->height; p++) {
		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

		pixDistances[0] = 1.0; pixDistances[1] = 1.0; pixDistances[2] = 1.0; // reset
		// pixColors[0] = NO_COLOR; pixColors[1] = NO_COLOR; pixColors[2] = NO_COLOR; // reset
		for(unsigned t = 0; t < colorPointTable->pointCount; t++){
			double dist = pix_dist(p, *(colorPoints + t), fieldImage->width) * (1.0 / (double)(fieldImage->width)); // distance multiplied by pixel size
			if(dist < pixDistances[0]){
				pixDistances[0] = dist;
				pixPoints[0] = (PixelPoint){ x - colorPointTable->points[t].x, y - colorPointTable->points[t].y };
				pixColors[0] = colorPointTable->points[t].color;
			} else if(dist < pixDistances[1]){
				pixDistances[1] = dist;
				pixPoints[1] = (PixelPoint){ x - colorPointTable->points[t].x, y - colorPointTable->points[t].y };
				pixColors[1] = colorPointTable->points[t].color;
			} else if(dist < pixDistances[2]){
				pixDistances[2] = dist;
				pixPoints[2] = (PixelPoint){ x - colorPointTable->points[t].x, y - colorPointTable->points[t].y };
				pixColors[2] = colorPointTable->points[t].color;
			}
		}

		*(fieldImage->data + p) = callback(pixColors, pixDistances, pixPoints);
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

Rasteron_Image* gradientImgOp(ImageSize size, enum SIDE_Type side, unsigned color1, unsigned color2){
	if(side == SIDE_None) return solidImgOp(size, color1);

	Rasteron_Image* gradientImg = RASTERON_ALLOC("gradient", size.height, size.width);

	for(unsigned p = 0; p < size.width * size.height; p++){
		double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

		if(side == SIDE_Left) *(gradientImg->data + p) = colors_blend(color1, color2, x);
		else if(side == SIDE_Right) *(gradientImg->data + p) = colors_blend(color1, color2, 1.0 - x);
		else if(side == SIDE_Top) *(gradientImg->data + p) = colors_blend(color1, color2, y);
		else if(side == SIDE_Bottom) *(gradientImg->data + p) = colors_blend(color1, color2, 1.0 - y);
		else {
			double centerDist = pix_dist(p, ((size.width * size.height) / 2) + (size.width / 2), size.width) * (1.0 / (double)size.width);
			*(gradientImg->data + p) = colors_blend(color1, color2, centerDist);
		}
	}


	return gradientImg;
}

Rasteron_Image* linedImgOp(ImageSize size, unsigned color1, unsigned color2, unsigned short divs, double rotation){
	Rasteron_Image* linedImg = RASTERON_ALLOC("lined", size.height, size.width);
	 
	for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

		float line;
		if(rotation == 0.0) line = (x * divs) - floor(x * divs);
		else if(fabs(rotation == 1.0)) line = (y * divs) - floor(y * divs);

		*(linedImg->data + p) = (line > 0.5)? color1 : color2;
    }

	return linedImg;
}

Rasteron_Image* checkeredImgOp(ImageSize size, ColorGrid grid){
// Rasteron_Image* checkeredImgOp(ImageSize size, Rasteron_Image* tileImages[2]){ // TODO: Replace with this
	Rasteron_Image* checkerImg = RASTERON_ALLOC("checker", size.height, size.width);

	for(unsigned p = 0; p < size.width * size.height; p++){
        double x = (1.0 / (double)size.width) * (p % size.width);
		double y = (1.0 / (double)size.height) * (p / size.width);

        unsigned c = x * grid.xCells;
        unsigned r = y * grid.yCells;

        if((c % 2 == 0 && r % 2 == 0) || (c % 2 == 1 && r % 2 == 1)) *(checkerImg->data + p) = grid.color1;
        else *(checkerImg->data + p) = grid.color2; 
    }

	// TODO: Change this to enable edges tiling

	return checkerImg;
}
