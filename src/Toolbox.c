/* Provide general image utilities */

#include "Toolbox.h"

// Reverses the blue and the red bits of the image
void switchBits_RB(uint32_t* raster, unsigned int pCount) {
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

void changeToSolidColor(uint32_t* raster, unsigned int pCount, uint32_t newColor, uint32_t oldColor) {
	for (unsigned int i = 0; i < pCount; i++)
		if (*(raster + i) == oldColor) 
			*(raster + i) = newColor;
	return;
}

// Produces a 32 bit grey value based on provided reference color
uint32_t grayify32(uint32_t refColor){
	if (refColor & 0x00FFFFFF == 0xFFFFFF) 
		return TOTAL_WHITE_COLOR_CODE;
	
	uint8_t alpha = 0xFF; // Complete opacity desired for alpha
	
	uint8_t avgColor = ((uint32_t)((refColor & 0xFF0000) << 16) + (uint32_t)((refColor & 0xFF00) << 16) + (uint32_t)((refColor & 0xFF) << 16)) / 3;

	uint32_t result = ((alpha << 24) | (avgColor << 16) | (avgColor << 8) | avgColor);
	return result;
}

// Produces a 8 bit grey value based on provided reference color
uint8_t grayify8(uint32_t refColor){
	if (refColor & 0x00FFFFFF == 0xFFFFFF)
		return 0xFF; 

	return ((uint32_t)((refColor & 0xFF0000) << 16) + (uint32_t)((refColor & 0xFF00) << 16) + (uint32_t)((refColor & 0xFF) << 16)) / 3;
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
