#include "Rasteron.h"

// Mixing Images

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
		else {
			xBounds[0] += (int)(coordX * maxX);
			xBounds[1] += (int)(coordX * maxX);
		}
	}
	if(coordY != 0.0){
		unsigned maxY = (unsigned)((outerImg->height / 2) - (innerImg->height / 2));
		if(coordY >= 1.0){ yBounds[0] += maxY; yBounds[1] += maxY; }
		else if(coordY <= -1.0){ yBounds[0] -= maxY; yBounds[1] -= maxY; }
		else {
			yBounds[0] += (int)(coordY * maxY);
			yBounds[1] += (int)(coordY * maxY);
		}
	}

	Rasteron_Image* insertImg = RASTERON_ALLOC("insert", outerImg->height, outerImg->width);
	unsigned ip = 0; // tracks inner image pixel position

	for(unsigned op = 0; op < outerImg->width * outerImg->height; op++){
		unsigned coordX = op % insertImg->width; // absolute X pixel offset
        unsigned coordY = op / insertImg->width; // absolute Y pixel offset
		if(coordX >= xBounds[0] && coordX < xBounds[1] && coordY >= yBounds[0] && coordY < yBounds[1]){
			uint8_t iAlphaVal = (*(innerImg->data + ip) & ALPHA_CHANNEL) >> 24;
			if(iAlphaVal == 0xFF) *(insertImg->data + op) = *(innerImg->data + ip);
			else *(insertImg->data + op) = blend_colors(*(innerImg->data + ip), *(outerImg->data + op), (double)(0xFF / iAlphaVal) );
			ip++;
		} else *(insertImg->data + op) = *(outerImg->data + op);
	}

	return insertImg;
}

Rasteron_Image* mixingImgOp(ref_image_t image1, ref_image_t image2, mixCallback callback){
	assert(image1 != NULL && image2 != NULL);

	Rasteron_Image* sizedImg1 = (image1->width * image1->height > image2->width * image2->height)? resizeImgOp((ImageSize){image2->height, image2->width}, image1) : copyImgOp(image1);
	Rasteron_Image* sizedImg2 = (image2->width * image2->height > image1->width * image1->height)? resizeImgOp((ImageSize){image1->height, image1->width}, image2) : copyImgOp(image2);

	Rasteron_Image* mixImage = RASTERON_ALLOC("mixing", sizedImg1->height, sizedImg1->width);
	for(unsigned p = 0; p < sizedImg1->width * sizedImg1->height; p++)
		*(mixImage->data + p) = callback(*(sizedImg1->data + p), *(sizedImg2->data + p));

	RASTERON_DEALLOC(sizedImg1);
	RASTERON_DEALLOC(sizedImg2);

	return mixImage;
}

Rasteron_Image* mixingExtImgOp(ref_image_t image1, ref_image_t image2, ref_image_t image3, ref_image_t image4, mixCallback4 callback){
	assert(image1 != NULL && image2 != NULL && image3 != NULL && image4 != NULL);

    unsigned maxWidth = 0;
    unsigned maxHeight = 0;
    if(image1->width > maxWidth) maxWidth = image1->width; if(image1->height > maxHeight) maxHeight = image1->height;
    if(image2->width > maxWidth) maxWidth = image2->width; if(image2->height > maxHeight) maxHeight = image2->height;
    if(image3->width > maxWidth) maxWidth = image3->width; if(image3->height > maxHeight) maxHeight = image3->height;
    if(image4->width > maxWidth) maxWidth = image4->width; if(image4->height > maxHeight) maxHeight = image4->height;
	
    Rasteron_Image* sizedImg1 = (image1->width == maxWidth && image1->height == maxHeight)? copyImgOp(image1) : resizeImgOp((ImageSize){ maxHeight, maxWidth }, image1);
    Rasteron_Image* sizedImg2 = (image2->width == maxWidth && image2->height == maxHeight)? copyImgOp(image2) : resizeImgOp((ImageSize){ maxHeight, maxWidth }, image2);
    Rasteron_Image* sizedImg3 = (image3->width == maxWidth && image3->height == maxHeight)? copyImgOp(image3) : resizeImgOp((ImageSize){ maxHeight, maxWidth }, image3);
    Rasteron_Image* sizedImg4 = (image4->width == maxWidth && image4->height == maxHeight)? copyImgOp(image4) : resizeImgOp((ImageSize){ maxHeight, maxWidth }, image4);

	Rasteron_Image* mixImage = RASTERON_ALLOC("mixing", sizedImg1->height, sizedImg1->width);
	for(unsigned p = 0; p < sizedImg1->width * sizedImg1->height; p++)
		*(mixImage->data + p) = callback(*(sizedImg1->data + p), *(sizedImg2->data + p), *(sizedImg3->data + p), *(sizedImg4->data + p));

	RASTERON_DEALLOC(sizedImg1);
	RASTERON_DEALLOC(sizedImg2);
	RASTERON_DEALLOC(sizedImg3);
	RASTERON_DEALLOC(sizedImg4);

    return mixImage;
}

Rasteron_Image* blendImgOp(ref_image_t image1, ref_image_t image2){
	return mixingImgOp(image1, image2, blend_colors_eq);
}

Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2){
	return mixingImgOp(image1, image2, fuse_colors_eq);
}

Rasteron_Image* warpingImgOp(ref_image_t refImage, ref_image_t domainImage){
	Rasteron_Image* sizedDomainImg = (domainImage->width * domainImage->height != refImage->width * refImage->height) 
		? resizeImgOp((ImageSize){ refImage->height, refImage->width }, domainImage) : copyImgOp(domainImage);
	Rasteron_Image* warpImage = copyImgOp(refImage);


	for(unsigned p = 0; p < warpImage->width * warpImage->height; p++){
        unsigned refColor = *(sizedDomainImg->data + p);
        double xOffset = (double)((refColor & RED_CHANNEL) >> 16) / 256.0;
        double yOffset = (double)((refColor & GREEN_CHANNEL) >> 8) / 256.0;
        double bLevel = (double)(refColor & BLUE_CHANNEL) / 256.0;

        PixelPoint point = { xOffset, yOffset };

        *(warpImage->data + p) = pixPoint_color(point, refImage); // adjust to x and y
        if(bLevel > 0.0) *(warpImage->data + p) = color_level(*(warpImage->data + p), bLevel); // adjust to brighness
    }

	RASTERON_DEALLOC(sizedDomainImg);

	return warpImage;
}
