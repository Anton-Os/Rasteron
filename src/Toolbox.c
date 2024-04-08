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


void bitSwitch_RB(uint32_t* data, unsigned pixels) {
	for (unsigned i = 0; i < pixels; i++) {
		unsigned val = *(data + i);
		unsigned res = ((val & 0xFF) << 16) + (val & 0xFF00) + ((val >> 16) & 0xFF);
		*(data + i) = res;
	}
}

void bitSwitch_GB(uint32_t* data, unsigned pixels) {
	for (unsigned i = 0; i < pixels; i++) {
		unsigned val = *(data + i);
		unsigned res = (val & 0xFF0000) + ((val & 0xFF) << 8) + ((val >> 8) & 0xFF);
		*(data + i) = res;
	}
}

void bitSwitch_RG(uint32_t* data, unsigned pixels) {
	for (unsigned i = 0; i < pixels; i++) {
		unsigned val = *(data + i);
		unsigned res = ((val & 0xFF00) << 8) + ((val >> 8) & 0xFF00) + (val & 0xFF);
		*(data + i) = res;
	}
}

uint8_t channel_low(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; shift = 24; break;
		case CHANNEL_Red: mask = RED_CHANNEL; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; shift = 0; break;
	}

	return ((color1 & mask) < (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

uint8_t channel_hi(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask, shift;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; shift = 24; break;
		case CHANNEL_Red: mask = RED_CHANNEL; shift = 16; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; shift = 8; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; shift = 0; break;
	}

	return ((color1 & mask) > (color2 & mask)) ? ((color1 & mask) >> shift) : ((color2 & mask) >> shift);
}

/* int8_t getLightDiff(uint32_t color1, uint32_t color2){ 
	return (int8_t)((int)grayify8(color1) - (int)grayify8(color2)); 
} */

int8_t channel_diff(uint32_t color1, uint32_t color2, CHANNEL_Type channel){
	uint32_t mask;
	switch(channel){
		case CHANNEL_Alpha: mask = ALPHA_CHANNEL; break;
		case CHANNEL_Red: mask = RED_CHANNEL; break;
		case CHANNEL_Green: mask = GREEN_CHANNEL; break;
		case CHANNEL_Blue: mask = BLUE_CHANNEL; break;
	}
	return (int8_t)((int)(color1 & mask) - (int)(color2 & mask));
}

uint32_t color_unique(){ // TODO: CHANGE START TO WHITE
	static unsigned invoke = 0; // increases each invocation
	unsigned color = 0xFFFFFFFF;

	for(unsigned i = 0; i < invoke; i++){
		if(i % 0xFF == 0xFE){
			color |= 0x000000FF; // &= 0xFFFFFF00; // reset blue bit
			color -= 0x100; // += 0x100; // increment green bit
		}
		if(i % 0xFFFF == 0xFFFE){
			color |= 0x0000FF00; // &= 0xFFFF00FF; // reset green bit
			color -= 0x10000; // += 0x10000; // increment red bit
		}
		color -= 0x1; // += 0x1; // increment blue bit
	}

	invoke++;
	return color;
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

uint32_t color_invert(uint32_t refColor){
	uint8_t alpha = (refColor & ALPHA_CHANNEL) >> 24;
	uint8_t red = 0xFF - ((refColor & RED_CHANNEL) >> 16);
	uint8_t green = 0xFF - ((refColor & GREEN_CHANNEL) >> 8);
	uint8_t blue = 0xFF - (refColor & BLUE_CHANNEL);

	uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	return result;
}

uint32_t colors_diff(uint32_t color1, uint32_t color2){
	if(color1 & 0x00FFFFFF > color2 & 0x00FFFFFF) return (color1 - color2) | (0xFF000000 & color1);
	else return (color2 - color1) | (0xFF000000 & color2);
}

uint32_t colors_entwine(uint32_t color1, uint32_t color2){ // enum CHANNEL_Type type){
	unsigned productColor = (0xFFFFFF & color1) * (0xFFFFFF & color2);
	unsigned spliceColor = ((unsigned)(pow((double)productColor, 0.5)) * 1) | 0xFF000000;
	// bitSwitch_RG(&spliceColor, 1);

	return spliceColor;
}

uint32_t colors_scramble(uint32_t color1, uint32_t color2, double pVal){
	uint8_t blueColor = (rand() / (double)RAND_MAX > pVal)? (color1 & 0xFF) : (color2 & 0xFF);
	uint8_t greenColor = (rand() / (double)RAND_MAX > pVal)? ((color1 & 0xFF00) >> 8) : ((color2 & 0xFF00) >> 8);
	uint8_t redColor = (rand() / (double)RAND_MAX > pVal)? ((color1 & 0xFF0000) >> 16) : ((color2 & 0xFF0000) >> 16);

	return (uint32_t)((0xFF << 24) + (redColor << 16) + (greenColor << 8) + blueColor);
}

uint32_t colors_blend(uint32_t color1, uint32_t color2, double bVal){
	if(bVal <= 0.0) return color1;
	else if(bVal >= 1.0) return color2;
	
	uint32_t bColor1 = fract32(color1, 1.0 -bVal);
	uint32_t bColor2 = fract32(color2, bVal);

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
	uint8_t finalRedBit = fract8((diffColor & RED_CHANNEL) >> 16, iVal);
	uint8_t finalGreenBit = fract8((diffColor & GREEN_CHANNEL) >> 8, iVal);
	uint8_t finalBlueBit = fract8(diffColor & BLUE_CHANNEL, iVal);

	return loColor + (uint32_t)((0xFF << 24) + (finalRedBit << 16) + (finalGreenBit << 8) + finalBlueBit);
}

uint32_t color_level(uint32_t color, double level){
	if(level >= 1.0) return 0xFFFFFFFF;
	else if(level <= 0.0) return 0xFF000000;
	else if(level == 0.5) return color;
	else if(level > 0.5) return colors_blend(color, 0xFFFFFFFF, (level - 0.5) * 2.0);
	else if(level < 0.5) return colors_blend(color, 0xFF000000, (0.5 - level) * 2.0);
	else return color;
}

double pix_dist(unsigned p1, unsigned p2, unsigned imageWidth){
	long int x1 = p1 % imageWidth;
	long int y1 = p1 / imageWidth;
	long int x2 = p2 % imageWidth;
	long int y2 = p2 / imageWidth;

	double w = fabs(x2 - x1); // length in pixels
	double l = fabs(y2 - y1); // width in pixels

	return (double)sqrt((l * l) + (w * w));
}

unsigned pixPoint_offset(PixelPoint pixPoint, ref_image_t refImage){
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

unsigned pixPoint_color(PixelPoint pixPos, ref_image_t refImage){
	return *(refImage->data + pixPoint_offset(pixPos, refImage));
}

unsigned pixPoint_cursorOffset(PixelPoint pixPoint, ref_image_t refImage){
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
	return pixPoint_offset(adjPoint, refImage);
}

unsigned pixPoint_cursorColor(PixelPoint cursorPos, ref_image_t refImage){
	return *(refImage->data + pixPoint_cursorOffset(cursorPos, refImage));
}