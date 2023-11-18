#include "Rasteron.h"

Rasteron_Image* insertImgOp(ref_image_t image1, ref_image_t image2, double coordX, double coordY){
	ref_image_t innerImg = (image1->width * image1->height < image2->width * image2->height)? image1 : image2; // (scale < 1.0)? image1 : image2; 
	ref_image_t outerImg = (image1->width * image1->height < image2->width * image2->height)? image2 : image1; // (scale < 1.0)? image2 : image1;

	unsigned midpoint[2] = { (outerImg->width / 2) - (innerImg->width / 2), (outerImg->height / 2) - (innerImg->height / 2) };
	unsigned xBounds[2] = { midpoint[0], midpoint[0] + innerImg->width };
	unsigned yBounds[2] = { midpoint[1], midpoint[1] + innerImg->height };
	if(coordX != 0.0){
		unsigned maxX = (unsigned)((outerImg->width / 2) - (innerImg->width / 2));
		if(coordX >= 1.0){ xBounds[0] += maxX; xBounds[1] += maxX; }
		else if(coordX <= -1.0){ xBounds[0] -= maxX; xBounds[1] -= maxX; }
		else{
			xBounds[0] += (coordX > 0.0)? (int)(coordX * maxX) : -(int)(coordX * maxX);
			xBounds[1] += (coordX > 0.0)? (int)(coordX * maxX) : -(int)(coordX * maxX);
		}
	}
	if(coordY != 0.0){
		unsigned maxY = (unsigned)((outerImg->height / 2) - (innerImg->height / 2));
		if(coordY >= 1.0){ yBounds[0] += maxY; yBounds[1] += maxY; }
		else if(coordY <= -1.0){ yBounds[0] -= maxY; yBounds[1] -= maxY; }
		else{
			yBounds[0] += (coordY > 0.0)? (int)(coordY * maxY) : -(int)(coordY * maxY);
			yBounds[1] += (coordY > 0.0)? (int)(coordY * maxY) : -(int)(coordY * maxY);
		}
	}

	Rasteron_Image* insertImg = alloc_image("insert", outerImg->height, outerImg->width);
	unsigned ip = 0; // tracks inner image pixel position

	for(unsigned op = 0; op < outerImg->width * outerImg->height; op++){
		unsigned coordX = op % insertImg->width; // absolute X pixel offset
        unsigned coordY = op / insertImg->width; // absolute Y pixel offset
		if(coordX >= xBounds[0] && coordX < xBounds[1] && coordY >= yBounds[0] && coordY < yBounds[1]){
			uint8_t iAlphaVal = (*(innerImg->data + ip) & ALPHA_CHANNEL) >> 24;
			if(iAlphaVal == 0xFF) 
				*(insertImg->data + op) = *(innerImg->data + ip);
			// else *(insertImg->data + op) = blendColors(*(innerImg->data + ip), *(outerImg->data + op), (double)(0xFF / iAlphaVal) );
			ip++;
		} else *(insertImg->data + op) = *(outerImg->data + op);
	}

	return insertImg;
}

Rasteron_Image* mixingImgOp(ref_image_t image1, ref_image_t image2, mixCallback callback){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* mixImage = alloc_image("mix", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(mixImage->data + p) = callback(*(image1->data + p), *(image2->data + p));

	return mixImage;
}


Rasteron_Image* blendImgOp(ref_image_t image1, ref_image_t image2){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* blendImage = alloc_image("blend", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(blendImage->data + p) = blendColors(*(image1->data + p), *(image2->data + p), 0.5); // blends halfway

	return blendImage;
}

Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2){
	assert(image1 != NULL && image2 != NULL && image1->width == image2->width && image1->height == image2->height);

	Rasteron_Image* fuseImage = alloc_image("fuse", image1->height, image1->width);
	for(unsigned p = 0; p < image1->width * image1->height; p++)
		*(fuseImage->data + p) = fuseColors(*(image1->data + p), *(image2->data + p), 0.5); // fuses halfway

	return fuseImage;
}