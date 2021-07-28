#include "Rasteron.h"

Rasteron_Image* allocNewImg(const char* name, uint32_t width, uint32_t height){
	Rasteron_Image* image = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	image->name = name;
	image->width = width;
	image->height = height;
	image->data = (uint32_t*)malloc(width * height * sizeof(uint32_t));

	return image;
}

Rasteron_Image* createImgBlank(uint32_t width, uint32_t height, uint32_t solidColor){
	Rasteron_Image* blankImage = allocNewImg("blank", width, height);

	makeRasterColor(blankImage->data, blankImage->width * blankImage->height, solidColor);

	return blankImage;
}

Rasteron_Image* createImgBase(const FileImage* refImage){
	if (refImage == NULL) {
		puts("Cannot create base image! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* baseImage;
    switch(refImage->fileFormat){
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		baseImage = allocNewImg("base", refImage->imageData.tiff.width, refImage->imageData.tiff.length);
		switchRasterRB(refImage->imageData.tiff.raster, refImage->imageData.tiff.width * refImage->imageData.tiff.length);
		for(unsigned i = 0; i < baseImage->width * baseImage->height; i++)
		   *(baseImage->data + i) = *(refImage->imageData.tiff.raster + i);
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		baseImage = allocNewImg("base", abs(refImage->imageData.bmp.width), abs(refImage->imageData.bmp.height));
		for (unsigned i = 0; i < baseImage->width * baseImage->height; i++)
			*(baseImage->data + i) = *(refImage->imageData.bmp.data + i);
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		baseImage = allocNewImg("base", refImage->imageData.png.width, refImage->imageData.png.height);
		for (unsigned i = 0; i < baseImage->width * baseImage->height; i++)
			*(baseImage->data + i) = *(refImage->imageData.png.rgbaData + i);
		break;
#endif
	default:
		puts("Image Format not yet supported!!!");
		break;
	}

	return baseImage;
}

Rasteron_Image* createImgGrey(const Rasteron_Image* refImage) {
	if (refImage == NULL) {
		puts("Cannot create grey image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* greyImage = allocNewImg("grey", refImage->width, refImage->height);
	
	// Generation Logic
	uint32_t grey;
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++) {
		grey = grayify32(*(refImage->data + p));
		*(greyImage->data + p) = grey;
	}

	return greyImage;
}

Rasteron_Image* createImgFilter(const Rasteron_Image* refImage, CHANNEL_Type channel) {
	if (refImage == NULL) {
		puts("Cannot create filter image! Null pointer provided as reference image!");
		return NULL;
	}

	Rasteron_Image* filterImage;
	uint32_t colorMask; // used for isolating a specific color value
	switch(channel){
		case CHANNEL_Red:
			filterImage = allocNewImg("red", refImage->width, refImage->height);
			colorMask = RED_BITS_MASK;
			break;
		case CHANNEL_Green:
			filterImage = allocNewImg("green", refImage->width, refImage->height);
			colorMask = GREEN_BITS_MASK;
			break;
		case CHANNEL_Blue:
			filterImage = allocNewImg("blue", refImage->width, refImage->height);
			colorMask = BLUE_BITS_MASK;
			break;
	}
	
	// Generation Logic
	uint32_t color;
	for (unsigned p = 0; p < filterImage->width * filterImage->height; p++) {
		color = (*(refImage->data + p) & ALPHA_BITS_MASK) + (*(refImage->data + p) & colorMask) ;
		*(filterImage->data + p) = color;
	}

	return filterImage;
}


void deleteImg(Rasteron_Image* image) {
    if(image->data != NULL) free(image->data);
    if(image != NULL) free(image);
}