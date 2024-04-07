#include "Rasteron.h"

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