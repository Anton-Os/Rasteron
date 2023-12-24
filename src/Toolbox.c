#include "Toolbox.h"

void seedRandGen(){ srand (time(NULL)); }

void replaceFwdSlash(char* str){
	unsigned len = strlen(str);
	for(unsigned l = 0; l < len; l++) if(*(str + l) == '/') *(str + l) = '\\'; // switch the dash type
}

/* uint32_t RAND_COLOR(){
	uint8_t redBit = rand() % 255;
	uint8_t greenBit = rand() % 255;
	uint8_t blueBit = rand() % 255;
	return (uint32_t)((0xFF << 24) + (redBit << 16) + (greenBit << 8) + blueBit);
} */


void bitSwitchRB(uint32_t* data, unsigned pixels) {
	for (unsigned i = 0; i < pixels; i++) {
		unsigned val = *(data + i);
		unsigned res = ((val & 0xFF) << 16) + (val & 0xFF00) + ((val >> 16) & 0xFF);
		*(data + i) = res;
	}
}

void bitSwitchGB(uint32_t* data, unsigned pixels) {
	for (unsigned i = 0; i < pixels; i++) {
		unsigned val = *(data + i);
		unsigned res = (val & 0xFF0000) + ((val & 0xFF) << 8) + ((val >> 8) & 0xFF);
		*(data + i) = res;
	}
}

void bitSwitchRG(uint32_t* data, unsigned pixels) {
	for (unsigned i = 0; i < pixels; i++) {
		unsigned val = *(data + i);
		unsigned res = ((val & 0xFF00) << 8) + ((val >> 8) & 0xFF00) + (val & 0xFF);
		*(data + i) = res;
	}
}

uint8_t getLoChanBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; shift = 24; break;
		case CHANNEL_Red: mask = RED_CHANNEL; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; shift = 0; break;
	}

	return ((color1 & mask) < (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

uint8_t getHiChanBit(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; shift = 24; break;
		case CHANNEL_Red: mask = RED_CHANNEL; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; shift = 0; break;
	}

	return ((color1 & mask) > (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

int8_t getLightDiff(uint32_t color1, uint32_t color2){ 
	return (int8_t)((int)grayify8(color1) - (int)grayify8(color2)); 
}

int8_t getChanDiff(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; break;
		case CHANNEL_Red: mask = RED_CHANNEL; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; break;
	}
	return (int8_t)((int)(color1 & mask) - (int)(color2 & mask));
}

uint32_t grayify32(uint32_t refColor) {
	if (refColor == WHITE_COLOR) return WHITE_COLOR;
	if (refColor == 0 || refColor == BLACK_COLOR) return BLACK_COLOR;
	
	uint8_t alpha = 0xFF; // Complete opacity desired for alpha
	uint8_t avgColor = (((refColor & RED_CHANNEL) >> 16) + ((refColor & 0xFF00) >> 8) + (refColor & 0xFF)) / 3;

	uint32_t result = ((alpha << 24) | (avgColor << 16) | (avgColor << 8) | avgColor);
	return result;
}

uint8_t grayify8(uint32_t refColor){
	if (refColor == WHITE_COLOR) return WHITE_COLOR;
	if (refColor == 0 || refColor == BLACK_COLOR) return BLACK_COLOR;

	uint8_t avgColor = (((refColor & RED_CHANNEL) >> 16) + ((refColor & 0xFF00) >> 8) + (refColor & 0xFF)) / 3;
	return avgColor;
}

uint8_t fract8(uint8_t refColor, double frac){
	if(frac >= 1.0) return refColor;
	else if(frac <= 0.0) return 0x00;
	else return ((double)refColor / 255.0) * frac * 255.0;
}

uint32_t fract32(uint32_t refColor, double frac){
	uint8_t alpha = fract8((refColor & ALPHA_CHANNEL) >> 24, frac);
	uint8_t red = fract8((refColor & RED_CHANNEL) >> 16, frac);
	uint8_t green = fract8((refColor & GREEN_CHANNEL) >> 8, frac);
	uint8_t blue = fract8(refColor & BLUE_CHANNEL, frac);

	uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	return result;
}

uint32_t invertColor(uint32_t refColor){
	uint8_t alpha = (refColor & ALPHA_CHANNEL) >> 24;
	uint8_t red = 0xFF - ((refColor & RED_CHANNEL) >> 16);
	uint8_t green = 0xFF - ((refColor & GREEN_CHANNEL) >> 8);
	uint8_t blue = 0xFF - (refColor & BLUE_CHANNEL);

	uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	return result;
}

uint32_t blendColors(uint32_t color1, uint32_t color2, double bVal){
	if(bVal <= 0.0) return color1;
	else if(bVal >= 1.0) return color2;
	
	uint32_t bColor1 = fract32(color1, 1.0 -bVal);
	uint32_t bColor2 = fract32(color2, bVal);

	return bColor1 + bColor2; 
}

uint32_t fuseColors(uint32_t color1, uint32_t color2, double iVal){
	uint8_t loRedBit = getLoChanBit(color1, color2, CHANNEL_Red); uint8_t hiRedBit = getHiChanBit(color1, color2, CHANNEL_Red);
    uint8_t loGreenBit = getLoChanBit(color1, color2, CHANNEL_Green); uint8_t hiGreenBit = getHiChanBit(color1, color2, CHANNEL_Green);
    uint8_t loBlueBit = getLoChanBit(color1, color2, CHANNEL_Blue); uint8_t hiBlueBit = getHiChanBit(color1, color2, CHANNEL_Blue);

	uint32_t loColor = (0 << 24) + (loRedBit << 16) + (loGreenBit << 8) + loBlueBit;
	if(iVal <= 0.0) return loColor;

	uint32_t hiColor = (0 << 24) + (hiRedBit << 16) + (hiGreenBit << 8) + hiBlueBit;
	if(iVal >= 1.0) return hiColor;

	uint32_t diffColor = (hiColor - loColor);
	uint8_t finalRedBit = fract8((diffColor & RED_CHANNEL) >> 16, iVal);
	uint8_t finalGreenBit = fract8((diffColor & GREEN_CHANNEL) >> 8, iVal);
	uint8_t finalBlueBit = fract8(diffColor & BLUE_CHANNEL, iVal);

	return loColor + (uint32_t)((0xFF << 24) + (finalRedBit << 16) + (finalGreenBit << 8) + finalBlueBit);
}

double pixelDistance(unsigned p1, unsigned p2, unsigned imageWidth){
	long int x1 = p1 % imageWidth;
	long int y1 = p1 / imageWidth;
	long int x2 = p2 % imageWidth;
	long int y2 = p2 / imageWidth;

	double w = abs(x2 - x1); // length in pixels
	double l = abs(y2 - y1); // width in pixels

	return (double)sqrt((l * l) + (w * w));
}

unsigned pixelPointOffset(PixelPoint pixPoint, ref_image_t refImage){
	unsigned xOffset; // clamping X
	if(pixPoint.x <= 0.0) xOffset = 0;
	else if(pixPoint.x >= 1.0) xOffset = refImage->width - 1;
	else xOffset = (unsigned)((double)refImage->width * pixPoint.x);

	unsigned yOffset; // clamping Y
	if(pixPoint.y <= 0.0) yOffset = 0;
	else if(pixPoint.y >= 1.0) yOffset = refImage->height - 1;
	else yOffset = (unsigned)((double)refImage->height * pixPoint.y);

	unsigned pixIndex = (yOffset * refImage->width) + xOffset;
	return pixIndex;
}

unsigned pixelPointColor(PixelPoint pixPos, ref_image_t refImage){
	return *(refImage->data + pixelPointOffset(pixPos, refImage));
}

unsigned pixelPointOffset_cursor(PixelPoint pixPoint, ref_image_t refImage){
	double x; // clamping X
	if(pixPoint.x <= -1.0) x = -1.0;
	else if(pixPoint.x >= 1.0) x = 1.0;
	else x = pixPoint.x;

	double y; // clamping Y
	if(pixPoint.y <= -1.0) y = -1.0;
	else if(pixPoint.y >= 1.0) y = 1.0;
	else y = pixPoint.y;
	y *= -1.0; // Y value needs to be flipped

	PixelPoint adjPoint = (PixelPoint){ (x / 2) + 0.5, (y / 2) + 0.5 };
	return pixelPointOffset(adjPoint, refImage);
}

unsigned pixelPointColor_cursor(PixelPoint cursorPos, ref_image_t refImage){
	return *(refImage->data + pixelPointOffset_cursor(cursorPos, refImage));
}