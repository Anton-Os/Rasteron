#ifndef RASTERON_FILTERING_H

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

Rasteron_Image* greyImgOp(ref_image_t refImage) {
	assert(refImage != NULL);
    
	Rasteron_Image* greyImage = RASTERON_ALLOC("grey", refImage->height, refImage->width);
	
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++)
		*(greyImage->data + p) = grayify32(*(refImage->data + p));

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
        double colorLevel = grayify8(*(splitImg->data + p)) / 256.0;
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

#define RASTERON_FILTERING_H
#endif
