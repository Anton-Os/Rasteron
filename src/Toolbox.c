/* Provide general image utilities */

#include "Toolbox.h"

void seedRandGen(){
    srand (time(NULL));
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

uint8_t getLoColorBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_BITS_MASK; shift = 24; break;
		case CHANNEL_Red: mask = RED_BITS_MASK; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_BITS_MASK; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_BITS_MASK; shift = 0; break;
	}

	return ((color1 & mask) < (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

uint8_t getHiColorBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_BITS_MASK; shift = 24; break;
		case CHANNEL_Red: mask = RED_BITS_MASK; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_BITS_MASK; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_BITS_MASK; shift = 0; break;
	}

	return ((color1 & mask) > (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}


uint32_t grayify32(uint32_t refColor){
	if (refColor & 0x00FFFFFF == 0xFFFFFF) 
		return TOTAL_WHITE_COLOR_CODE;
	
	uint8_t alpha = 0xFF; // Complete opacity desired for alpha
	
	uint8_t avgColor = ((uint32_t)((refColor & 0xFF0000) << 16) + (uint32_t)((refColor & 0xFF00) << 16) + (uint32_t)((refColor & 0xFF) << 16)) / 3;

	uint32_t result = ((alpha << 24) | (avgColor << 16) | (avgColor << 8) | avgColor);
	return result;
}

uint8_t grayify8(uint32_t refColor){
	if (refColor & 0x00FFFFFF == 0xFFFFFF)
		return 0xFF; 

	return ((uint32_t)((refColor & 0xFF0000) << 16) + (uint32_t)((refColor & 0xFF00) << 16) + (uint32_t)((refColor & 0xFF) << 16)) / 3;
}

uint8_t fract8(uint8_t refColor, double frac){
	if(frac >= 1.0) return refColor;
	else if(frac <= 0.0) return 0x00;
	else return ((double)refColor / 255.0) * frac * 255.0;
}

// Interpolates between 2 colors
uint32_t interpolateColor(uint32_t color1, uint32_t color2, double iVal){

	uint8_t loAlphaBit = getLoColorBit(color1, color2, CHANNEL_Alpha);
	uint8_t loRedBit = getLoColorBit(color1, color2, CHANNEL_Red);
    uint8_t loGreenBit = getLoColorBit(color1, color2, CHANNEL_Green);
    uint8_t loBlueBit = getLoColorBit(color1, color2, CHANNEL_Blue);
	uint8_t hiAlphaBit = getHiColorBit(color1, color2, CHANNEL_Alpha);
    uint8_t hiRedBit = getHiColorBit(color1, color2, CHANNEL_Red);
    uint8_t hiGreenBit = getHiColorBit(color1, color2, CHANNEL_Green);
    uint8_t hiBlueBit = getHiColorBit(color1, color2, CHANNEL_Blue);

	uint32_t loColor = (loAlphaBit << 24) + (loRedBit << 16) + (loGreenBit << 8) + loBlueBit;
	if(iVal <= 0.0) return loColor;

	uint32_t hiColor = (hiAlphaBit << 24) + (hiRedBit << 16) + (hiGreenBit << 8) + hiBlueBit;
	if(iVal <= 0.0) return hiColor;

	uint32_t diffColor = (hiColor - loColor);
	uint8_t finalAlphaBit = 0xFF;
	uint8_t finalRedBit = fract8((diffColor & RED_BITS_MASK) >> 16, iVal);
	uint8_t finalGreenBit = fract8((diffColor & GREEN_BITS_MASK) >> 8, iVal);
	uint8_t finalBlueBit = fract8(diffColor & BLUE_BITS_MASK, iVal);

	return loColor + (uint32_t)((finalAlphaBit << 24) + (finalRedBit << 16) + (finalGreenBit << 8) + finalBlueBit);
}