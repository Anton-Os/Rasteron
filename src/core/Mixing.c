// --------------------------------   Support Functions   -------------------------------- //

#include "support_def.h"

uint32_t colors_blend(uint32_t color1, uint32_t color2, double bVal){
	if(bVal <= 0.0) return color1;
	else if(bVal >= 1.0) return color2;
	
	uint32_t bColor1 = color_fractional(color1, 1.0 -bVal);
	uint32_t bColor2 = color_fractional(color2, bVal);

	return bColor1 + bColor2; 
}

uint32_t colors_fuse(uint32_t color1, uint32_t color2, double iVal){
	uint8_t loRedBit = channel_low(color1, color2, CHANNEL_Red); uint8_t hiRedBit = channel_hi(color1, color2, CHANNEL_Red);
    uint8_t loGreenBit = channel_low(color1, color2, CHANNEL_Green); uint8_t hiGreenBit = channel_hi(color1, color2, CHANNEL_Green);
    uint8_t loBlueBit = channel_low(color1, color2, CHANNEL_Blue); uint8_t hiBlueBit = channel_hi(color1, color2, CHANNEL_Blue);

	uint32_t loColor = (0 << 24) + (loRedBit << 16) + (loGreenBit << 8) + loBlueBit;
	if(iVal <= 0.0) return loColor;

	uint32_t hiColor = (0 << 24) + (hiRedBit << 16) + (hiGreenBit << 8) + hiBlueBit;
	if(iVal >= 1.0) return hiColor;

	uint32_t diffColor = (hiColor - loColor);
	uint8_t finalRedBit = channel_fractional((diffColor & RED_CHANNEL) >> 16, iVal);
	uint8_t finalGreenBit = channel_fractional((diffColor & GREEN_CHANNEL) >> 8, iVal);
	uint8_t finalBlueBit = channel_fractional(diffColor & BLUE_CHANNEL, iVal);

	return loColor + (uint32_t)((0xFF << 24) + (finalRedBit << 16) + (finalGreenBit << 8) + finalBlueBit);
}

uint32_t colors_diff(uint32_t color1, uint32_t color2){
	if(color1 & 0x00FFFFFF > color2 & 0x00FFFFFF) return (color1 - color2) | (0xFF000000 & color1);
	else return (color2 - color1) | (0xFF000000 & color2);
}

uint32_t colors_powroot(uint32_t color1, uint32_t color2){ // enum CHANNEL_Type type){
	unsigned product = (0xFFFFFF & color1) * (0xFFFFFF & color2); // power
	unsigned color = ((unsigned)(pow((double)product, 0.5)) * 1) | 0xFF000000; // root
	// bitSwitch_RG(&color, 1);

	return color;
}


// --------------------------------   Mixing Operations    -------------------------------- //

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
			else *(insertImg->data + op) = colors_blend(*(innerImg->data + ip), *(outerImg->data + op), (double)(0xFF / iAlphaVal) );
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

	unsigned minWidth = -1;
	unsigned minHeight = -1;
	if(image1->width < minWidth) minWidth = image1->width; if(image1->height < minHeight) minHeight = image1->height;
	if(image2->width < minWidth) minWidth = image2->width; if(image2->height < minHeight) minHeight = image2->height;
	if(image3->width < minWidth) minWidth = image3->width; if(image3->height < minHeight) minHeight = image3->height;
	if(image4->width < minWidth) minWidth = image4->width; if(image4->height < minHeight) minHeight = image4->height;
	
	Rasteron_Image* sizedImg1 = (image1->width == minWidth && image1->height == minHeight)? copyImgOp(image1) : resizeImgOp((ImageSize){ minHeight, minWidth }, image1);
	Rasteron_Image* sizedImg2 = (image2->width == minWidth && image2->height == minHeight)? copyImgOp(image2) : resizeImgOp((ImageSize){ minHeight, minWidth }, image2);
	Rasteron_Image* sizedImg3 = (image3->width == minWidth && image3->height == minHeight)? copyImgOp(image3) : resizeImgOp((ImageSize){ minHeight, minWidth }, image3);
	Rasteron_Image* sizedImg4 = (image4->width == minWidth && image4->height == minHeight)? copyImgOp(image4) : resizeImgOp((ImageSize){ minHeight, minWidth }, image4);

	Rasteron_Image* mixImage = RASTERON_ALLOC("mixing", sizedImg1->height, sizedImg1->width);
	for(unsigned p = 0; p < sizedImg1->width * sizedImg1->height; p++)
		*(mixImage->data + p) = callback(*(sizedImg1->data + p), *(sizedImg2->data + p), *(sizedImg3->data + p), *(sizedImg4->data + p));

	RASTERON_DEALLOC(sizedImg1);
	RASTERON_DEALLOC(sizedImg2);
	RASTERON_DEALLOC(sizedImg3);
	RASTERON_DEALLOC(sizedImg4);

	return errorImgOp("Not supported yet");
}

static unsigned equalBlend(unsigned color1, unsigned color2){ return colors_blend(color1, color2, 0.5); }

Rasteron_Image* blendImgOp(ref_image_t image1, ref_image_t image2){
	return mixingImgOp(image1, image2, equalBlend);
}

static unsigned equalFusion(unsigned color1, unsigned color2){ return colors_blend(color1, color2, 0.5); }

Rasteron_Image* fusionImgOp(ref_image_t image1, ref_image_t image2){
	return mixingImgOp(image1, image2, equalFusion);
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