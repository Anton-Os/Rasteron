/* Provide general image utilities */

#include "ImageLoader.h"

static enum IMG_FileFormat getFileExtension(const char* filePath) {
	return IMG_NonValid;
}

// Reverses the blue and the red bits of the image
void revrsColorBits_RB(uint32_t* raster, unsigned int pCount) {
	unsigned int bCount = pCount * 8;
	for (unsigned int i = 0; i < pCount; i++) {
		unsigned int val = *(raster + i);
		unsigned int res = ((val & 0xFF) << 16) + (val & 0xFF00) + ((val >>  16) & 0xFF);
		*(raster + i) = res;
	}
	return;
}

void makeSolidColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal) {
	for (unsigned int i = 0; i < pCount; i++)
		*(raster + i) = colorVal;
	return;
}

void changeSolidColor(uint32_t* raster, unsigned int pCount, uint32_t newClr, uint32_t oldClr) {
	for (unsigned int i = 0; i < pCount; i++)
		if (*(raster + i) == oldClr) 
			*(raster + i) = newClr;
	return;
}

// Produces a 32 bit grey value based on provided reference color
uint32_t grayify_32(uint32_t refClr){ // Needs fixing!!!
	uint8_t alpha = 0xFF; // Complete opacity for now
	
	uint8_t avgClr = ((uint32_t)(refClr & 0xFF0000) + (uint32_t)(refClr & 0xFF00) + (uint32_t)(refClr & 0xFF)) / 3;
	uint32_t result = ((alpha << 24) | (avgClr << 16) | (avgClr << 8) | avgClr);
	return result;
	// return (uint32_t)((alpha << 24) | (greyVal << 16) | (greyVal << 8) | greyVal);
}

// Produces a 8 bit grey value based on provided reference color
uint8_t grayify_8(uint32_t refClr){
	uint8_t greyVal = ((uint32_t)(refClr & 0xFF) + (uint32_t)(refClr & 0xFF00) + (uint32_t)(refClr & 0xFF0000)) / 3;
	return greyVal;
}

uint32_t getBkColor(uint32_t* raster, unsigned width, unsigned height){
	unsigned topLeftPix = *(raster + 0);
	unsigned topRightPix = *(raster + width - 1);
	unsigned botLeftPix = *(raster + (width * height) - width - 1);
	unsigned botRightPix = *(raster + (width * height) - 1);

	if(topLeftPix == topRightPix && botLeftPix == botRightPix && topRightPix == botRightPix)
		return topLeftPix; // Chose arbitrarily
	else {
		puts("Could not find a background color!");
		return BAD_COLOR_CODE;
	}
}
