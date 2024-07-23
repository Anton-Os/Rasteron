// --------------------------------   Support Functions   -------------------------------- //

#include "support_def.h"

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
	return (int8_t)((int)channel_grayscale(color1) - (int)channel_grayscale(color2)); 
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

uint32_t color_grayscale(uint32_t refColor) {
    if (refColor == WHITE_COLOR) return WHITE_COLOR;
    if (refColor == 0 || refColor == BLACK_COLOR) return BLACK_COLOR;

    uint8_t alpha = 0xFF; // Complete opacity desired for alpha
    uint8_t avgColor = (((refColor & RED_CHANNEL) >> 16) + ((refColor & 0xFF00) >> 8) + (refColor & 0xFF)) / 3;

    uint32_t result = ((alpha << 24) | (avgColor << 16) | (avgColor << 8) | avgColor);
    return result;
}

uint8_t channel_grayscale(uint32_t refColor){
    if (refColor == WHITE_COLOR) return WHITE_COLOR;
    if (refColor == 0 || refColor == BLACK_COLOR) return BLACK_COLOR;

    uint8_t avgColor = (((refColor & RED_CHANNEL) >> 16) + ((refColor & 0xFF00) >> 8) + (refColor & 0xFF)) / 3;
    return avgColor;
}

// --------------------------------   Filtering Operations    -------------------------------- //

#include "Rasteron.h"

Rasteron_Image* recolorImgOp(ref_image_t refImage, recolorCallback callback){
	assert(refImage != NULL);
    
    Rasteron_Image* recolorImage = RASTERON_ALLOC("recolor", refImage->height, refImage->width);

	for(unsigned p = 0; p < recolorImage->width * recolorImage->height; p++){
		unsigned newColor = callback(*(refImage->data + p));
		if(newColor != NO_COLOR) *(recolorImage->data + p) = newColor; // override color
		else *(recolorImage->data + p) = *(refImage->data + p); // preserve
	}

	return recolorImage;
}

Rasteron_Image* remaskImgOp(ref_image_t refImage, remaskCallback callback){
	assert(refImage != NULL);
    
    Rasteron_Image* remaskImage = RASTERON_ALLOC("remask", refImage->height, refImage->width);

	for(unsigned p = 0; p < remaskImage->width * remaskImage->height; p++){
		unsigned refColor = *(refImage->data + p);

		uint8_t alpha = callback(CHANNEL_Alpha, (refColor & ALPHA_CHANNEL) >> 24);
		uint8_t red = callback(CHANNEL_Red, (refColor & RED_CHANNEL) >> 16);
		uint8_t green = callback(CHANNEL_Green, (refColor & GREEN_CHANNEL) >> 8);
		uint8_t blue = callback(CHANNEL_Blue, refColor & BLUE_CHANNEL);

		*(remaskImage->data + p) = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	}

	return remaskImage;
}

Rasteron_Image* greyImgOp(ref_image_t refImage) {
	assert(refImage != NULL);
    
	Rasteron_Image* greyImage = RASTERON_ALLOC("grey", refImage->height, refImage->width);
	
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++)
		*(greyImage->data + p) = color_grayscale(*(refImage->data + p));

	return greyImage;
}

Rasteron_Image* filterImgOp(ref_image_t refImage, CHANNEL_Type channel) {
	assert(refImage != NULL);

	Rasteron_Image* channelImage;
	uint32_t colorMask; // used for isolating a specific color value
	switch(channel){
		case CHANNEL_Red:
			channelImage = RASTERON_ALLOC("filter-red", refImage->height, refImage->width);
			colorMask = RED_CHANNEL;
			break;
		case CHANNEL_Green:
			channelImage = RASTERON_ALLOC("filter-green", refImage->height, refImage->width);
			colorMask = GREEN_CHANNEL;
			break;
		case CHANNEL_Blue:
			channelImage = RASTERON_ALLOC("filter-blue", refImage->height, refImage->width);
			colorMask = BLUE_CHANNEL;
			break;
	}
	
	uint32_t color;
	for (unsigned p = 0; p < channelImage->width * channelImage->height; p++) {
		color = (*(refImage->data + p) & ALPHA_CHANNEL) + (*(refImage->data + p) & colorMask) ;
		*(channelImage->data + p) = color;
	}

	return channelImage;
}

Rasteron_Image* channelImgOp(ref_image_t refImage, CHANNEL_Type channel) {
	assert(refImage != NULL);

	Rasteron_Image* greyImage = greyImgOp(refImage); // greyscale image used as reference
	Rasteron_Image* channelImage = filterImgOp(greyImage, channel);

	switch(channel){ // renaming
		case CHANNEL_Red: channelImage->name = "channel-red"; break;
		case CHANNEL_Green: channelImage->name = "channel-green"; break;
		case CHANNEL_Blue: channelImage->name = "channel-blue"; break;
	}

	RASTERON_DEALLOC(greyImage);
	return channelImage;
}

Rasteron_Image* splitImgOp(ref_image_t refImage, unsigned short levels){
	Rasteron_Image* splitImg = copyImgOp(refImage);

	for(unsigned p = 0; p < splitImg->width * splitImg->height; p++){
        double colorLevel = channel_grayscale(*(splitImg->data + p)) / 256.0;
        double adjustLevel = 1.0;
        
        for(unsigned l = 0; l < levels; l++)
            if(fabs((l * (1.0 / levels)) - colorLevel) < fabs(adjustLevel - colorLevel))
                adjustLevel = l * (1.0 / levels);

        *(splitImg->data + p) = color_level(*(splitImg->data + p), adjustLevel);
    }

	return splitImg;
}

Rasteron_Image* colorSwitchImgOp(ref_image_t refImage, CHANNEL_Type channel1, CHANNEL_Type channel2){
	assert(refImage != NULL);
	if(channel1 == channel2 || channel1 == CHANNEL_Alpha || channel2 == CHANNEL_Alpha) return copyImgOp(refImage);
    
    Rasteron_Image* colorSwitchImg = copyImgOp(refImage);

	if((channel1 == CHANNEL_Red && channel2 == CHANNEL_Blue) || (channel2 == CHANNEL_Red && channel1 == CHANNEL_Blue))
		bitSwitch_RB(colorSwitchImg->data, colorSwitchImg->width * colorSwitchImg->height);
	if((channel1 == CHANNEL_Red && channel2 == CHANNEL_Green) || (channel2 == CHANNEL_Red && channel1 == CHANNEL_Green))
		bitSwitch_RG(colorSwitchImg->data, colorSwitchImg->width * colorSwitchImg->height);
	if((channel1 == CHANNEL_Green && channel2 == CHANNEL_Blue) || (channel2 == CHANNEL_Green && channel1 == CHANNEL_Blue))
		bitSwitch_GB(colorSwitchImg->data, colorSwitchImg->width * colorSwitchImg->height);

	return colorSwitchImg;
}

Rasteron_Image* colorShiftImgOp(ref_image_t refImage, short redShift, short greenShift, short blueShift){
	assert(refImage != NULL);
	if(redShift == 0 && greenShift == 0 && blueShift == 0) return copyImgOp(refImage);
    
    Rasteron_Image* colorShiftImg = copyImgOp(refImage);

	for(unsigned p = 0; p < colorShiftImg->width * colorShiftImg->height; p++){
		unsigned refColor = *(colorShiftImg->data + p);
		unsigned newColor = refColor & 0xFF000000;
		// TODO: Determine red from redShift
		// TODO: Determine green fron greenShift
		// TODO: Determine blue from blueShift
		*(colorShiftImg->data + p) = newColor;
	}

	return colorShiftImg;
}