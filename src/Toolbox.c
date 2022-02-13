/* Image utilities and helper functions */

#include "Toolbox.h"

void fixPathDashes(char* str){
	unsigned len = strlen(str);
	for(unsigned l = 0; l < len; l++) if(*(str + l) == '/') *(str + l) = '\\'; // switch the dash type
}

void seedRandGen(){ srand (time(NULL)); }

uint32_t genRandColorVal(){
	uint8_t redBit = rand() % 255;
	uint8_t greenBit = rand() % 255;
	uint8_t blueBit = rand() % 255;
	return (uint32_t)((0xFF << 24) + (redBit << 16) + (greenBit << 8) + blueBit);
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

double getPixDist(unsigned p1, unsigned p2, unsigned imageWidth){
	unsigned x1 = p1 % imageWidth;
    unsigned y1 = p1 / imageWidth;
	unsigned x2 = p2 % imageWidth;
	unsigned y2 = p2 / imageWidth;
	return sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
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

uint32_t fract32(uint32_t refColor, double frac){
	uint8_t alpha = fract8((refColor & ALPHA_BITS_MASK) >> 24, frac);
	uint8_t red = fract8((refColor & RED_BITS_MASK) >> 16, frac);
	uint8_t green = fract8((refColor & GREEN_BITS_MASK) >> 8, frac);
	uint8_t blue = fract8(refColor & BLUE_BITS_MASK, frac);

	uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	return result;
}

uint32_t itrpolate(uint32_t color1, uint32_t color2, double iVal){
	uint8_t loRedBit = getLoColorBit(color1, color2, CHANNEL_Red); uint8_t hiRedBit = getHiColorBit(color1, color2, CHANNEL_Red);
    uint8_t loGreenBit = getLoColorBit(color1, color2, CHANNEL_Green); uint8_t hiGreenBit = getHiColorBit(color1, color2, CHANNEL_Green);
    uint8_t loBlueBit = getLoColorBit(color1, color2, CHANNEL_Blue); uint8_t hiBlueBit = getHiColorBit(color1, color2, CHANNEL_Blue);

	uint32_t loColor = (0 << 24) + (loRedBit << 16) + (loGreenBit << 8) + loBlueBit;
	if(iVal <= 0.0) return loColor;

	uint32_t hiColor = (0 << 24) + (hiRedBit << 16) + (hiGreenBit << 8) + hiBlueBit;
	if(iVal >= 1.0) return hiColor;

	uint32_t diffColor = (hiColor - loColor);
	uint8_t finalRedBit = fract8((diffColor & RED_BITS_MASK) >> 16, iVal);
	uint8_t finalGreenBit = fract8((diffColor & GREEN_BITS_MASK) >> 8, iVal);
	uint8_t finalBlueBit = fract8(diffColor & BLUE_BITS_MASK, iVal);

	return loColor + (uint32_t)((0xFF << 24) + (finalRedBit << 16) + (finalGreenBit << 8) + finalBlueBit);
}

uint32_t blend(uint32_t color1, uint32_t color2, double bVal){
	if(bVal <= 0.0) return color1;
	else if(bVal >= 1.0) return color2;
	
	uint32_t bColor1 = fract32(color1, 1.0 -bVal);
	uint32_t bColor2 = fract32(color2, bVal);

	return bColor1 + bColor2; 
}