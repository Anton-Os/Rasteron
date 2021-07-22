#include "Rasteron.h"

Rasteron_Image* createImgBase(const Image* refImage){
	if (refImage == NULL) {
		puts("Cannot create base image! Null pointer provided as reference image!");
		return NULL;
	}

    Rasteron_Image* baseImage = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
    baseImage->name = "base";

    switch(refImage->fileFormat){

#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		baseImage->width = refImage->imageData.tiff.width;
        baseImage->height = refImage->imageData.tiff.length;
        baseImage->data = (uint32_t*)malloc(baseImage->width * baseImage->height * sizeof(uint32_t));
        
		switchBits_RB(refImage->imageData.tiff.raster, refImage->imageData.tiff.width * refImage->imageData.tiff.length);
		for(unsigned i = 0; i < baseImage->width * baseImage->height; i++)
		   *(baseImage->data + i) = *(refImage->imageData.tiff.raster + i); // Fix the color inversion!!!
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		baseImage->width = abs(refImage->imageData.bmp.width);
        baseImage->height = abs(refImage->imageData.bmp.height);
        baseImage->data = (uint32_t*)malloc(baseImage->width * baseImage->height * sizeof(uint32_t));

		for (unsigned i = 0; i < baseImage->width * baseImage->height; i++)
			*(baseImage->data + i) = *(refImage->imageData.bmp.data + i);
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		baseImage->width = refImage->imageData.png.width;
        baseImage->height = refImage->imageData.png.height;
        baseImage->data = (uint32_t*)malloc(baseImage->width * baseImage->height * sizeof(uint32_t));
		
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

	Rasteron_Image* greyImage = (Rasteron_Image*)malloc(sizeof(Rasteron_Image));
	greyImage->name = "grey";

	greyImage->height = refImage->height;
	greyImage->width = refImage->width;
	greyImage->data = (uint32_t*)malloc(greyImage->width * greyImage->height * sizeof(uint32_t));
	
	// Grey Image Logic
	uint32_t grey;
	for (unsigned p = 0; p < greyImage->width * greyImage->height; p++) {
		unsigned pixColorDbg = *(refImage->data + p); // For testing

		grey = (uint32_t)grayify32(*(refImage->data + p));
		*(greyImage->data + p) = grey;
	}

	return greyImage;
}

void deleteImg(Rasteron_Image* image) {
    free(image->data);
    free(image);
}