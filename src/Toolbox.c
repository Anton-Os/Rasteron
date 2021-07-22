/* Provide general image utilities */

#include "Toolbox.h"

void switchRasterRB(uint32_t* raster, unsigned int pCount) {
	for (unsigned int i = 0; i < pCount; i++) {
		unsigned int val = *(raster + i);
		unsigned int res = ((val & 0xFF) << 16) + (val & 0xFF00) + ((val >> 16) & 0xFF);
		// unsigned int res = (val & RED_BITS_MASK) + (val & GREEN_BITS_MASK) + (val & BLUE_BITS_MASK);
		*(raster + i) = res;
	}
}

void switchRasterGB(uint32_t* raster, unsigned int pCount) {
	for (unsigned int i = 0; i < pCount; i++) {
		unsigned int val = *(raster + i);
		unsigned int res = (val & 0xFF0000) + ((val & 0xFF) << 8) + ((val >> 8) & 0xFF);
		*(raster + i) = res;
	}
}

void switchRasterRG(uint32_t* raster, unsigned int pCount) {
	for (unsigned int i = 0; i < pCount; i++) {
		unsigned int val = *(raster + i);
		unsigned int res = ((val & 0xFF00) << 8) + ((val >> 8) & 0xFF00) + (val & 0xFF);
		*(raster + i) = res;
	}
}

void makeRasterColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal) {
	for (unsigned int i = 0; i < pCount; i++)
		*(raster + i) = colorVal;
}

void changeRasterColor(uint32_t* raster, unsigned int pCount, uint32_t newColor, uint32_t oldColor) {
	for (unsigned int i = 0; i < pCount; i++)
		if (*(raster + i) == oldColor) 
			*(raster + i) = newColor;
}

uint32_t getRasterBkColor(uint32_t* raster, unsigned width, unsigned height){
	// TODO: Implement this
	return BAD_COLOR_CODE;
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