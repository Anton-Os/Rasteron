#include "Rasteron.h"

Rasteron_Image* loadImgOp(const char* fileName){
	FileImage fileImage;
	loadFromFile(fileName, &fileImage);
	if(&fileImage == NULL) return NULL; // Image not loaded

    Rasteron_Image* refImage = NULL;
    switch(fileImage.fileFormat){
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		refImage = alloc_image("ref-tiff", fileImage.data.tiff.length, fileImage.data.tiff.width);
		bitSwitchRB(fileImage.data.tiff.data, fileImage.data.tiff.width * fileImage.data.tiff.length);
		for(unsigned i = 0; i < refImage->width * refImage->height; i++)
		   *(refImage->data + i) = *(fileImage.data.tiff.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		refImage = alloc_image("ref-bmp", abs(fileImage.data.bmp.height), abs(fileImage.data.bmp.width));
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			*(refImage->data + i) = *(fileImage.data.bmp.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		refImage = alloc_image("ref-png", fileImage.data.png.width, fileImage.data.png.height);
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			*(refImage->data + i) = *(fileImage.data.png.data + i); // copying operation
		break;
#endif
	}

	delFileImage(&fileImage);
	return refImage;
}

Rasteron_Image* solidImgOp(ImageSize size, uint32_t color){
	Rasteron_Image* solidImage = alloc_image("solid", size.height, size.width);

	for (unsigned i = 0; i < solidImage->height * solidImage->width; i++)
		*(solidImage->data + i) = color;

	return solidImage;
}

Rasteron_Image* copyedImgOp(ref_image_t refImage){
	assert(refImage != NULL);

	Rasteron_Image* copyImage = alloc_image("copy", refImage->height, refImage->width);

	for(unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(copyImage->data + p) = *(refImage->data + p); // copy each pixel

	return copyImage;
}

Rasteron_Image* mirroredImgOp(ref_image_t refImage){
	assert(refImage != NULL);

	Rasteron_Image* mirrorImage = copyedImgOp(refImage);

	for (unsigned r = 0; r < refImage->height; r++) // Mirror Function
		for (unsigned c = 0; c < refImage->width; c++) {
			*(mirrorImage->data + (r * refImage->width) + c) = *(refImage->data + ((r + 1) * refImage->width) - c - 1);
			*(mirrorImage->data + ((r + 1) * refImage->width) - c - 1) = *(refImage->data + (r * refImage->width) + c); // mirror right pixel
		}
	
	return mirrorImage;
}

Rasteron_Image* flippedImgOp(ref_image_t refImage, enum FLIP_Type flip){
	assert(refImage != NULL);
	
	Rasteron_Image* flipImage = NULL;
	if(flip == FLIP_Upside){
		flipImage = alloc_image("flip", refImage->height, refImage->width); // parameters match source
		for(unsigned p = 0; p < refImage->height * refImage->width; p++)
			*(flipImage->data + (refImage->height * refImage->width) - p - 1) = *(refImage->data + p); // copies pixels in reverse
	} else { // TODO: Fix Orientation
		flipImage = alloc_image("flip", refImage->width, refImage->height); // parameters inverse of source
		unsigned offset = 0;

		for(unsigned w = 0; w < refImage->width; w++){
			unsigned match = (flip == FLIP_Clock) 
				? (refImage->width * refImage->height) + w
				: refImage->width - w - 1;
	
			for(unsigned h = 0; h < refImage->height; h++){
				match = (flip == FLIP_Clock) ? match - refImage->width : match + refImage->width;
				*(flipImage->data + offset) = *(refImage->data + match); // copies to image from match index

				offset++; // move to next pixel
			}
		}
	}

	return flipImage;
}