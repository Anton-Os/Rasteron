#include "Toolbox.h"

#include "Rasteron.h"


Rasteron_Image* loadImgOp(const char* fileName){
	FileImage fileImage;
#ifdef _WIN32
	replaceFwdSlash(fileName);
#endif
	loadFromFile(fileName, &fileImage);
	if(&fileImage == NULL) return NULL; // Image not loaded

    Rasteron_Image* refImage = NULL;
    switch(fileImage.fileFormat){
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		refImage = alloc_image("tiff", fileImage.data.tiff.length, fileImage.data.tiff.width);
		bitSwitchRB(fileImage.data.tiff.data, fileImage.data.tiff.width * fileImage.data.tiff.length);
		for(unsigned i = 0; i < refImage->width * refImage->height; i++)
		   // *(refImage->data + (refImage->width * refImage->height) - 1 - i) = *(fileImage.data.tiff.data + i); // copying operation
		   *(refImage->data + i) = *(fileImage.data.tiff.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		refImage = alloc_image("bmp", abs(fileImage.data.bmp.height), abs(fileImage.data.bmp.width));
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			// *(refImage->data + (refImage->width * refImage->height) - 1 - i) = *(fileImage.data.bmp.data + i); // copying operation
			*(refImage->data + i) = *(fileImage.data.bmp.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		refImage = alloc_image("png", fileImage.data.png.height, fileImage.data.png.width);
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			// *(refImage->data + (refImage->width * refImage->height) - 1 - i) = *(fileImage.data.png.data + i); // copying operation
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

Rasteron_Image* copyImgOp(ref_image_t refImage){
	assert(refImage != NULL);

	Rasteron_Image* copyImage = alloc_image("copy", refImage->height, refImage->width);

	for(unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(copyImage->data + p) = *(refImage->data + p); // copy each pixel

	return copyImage;
}

Rasteron_Image* resizeImgOp(ImageSize size, ref_image_t refImage){
	assert(refImage != NULL);

	Rasteron_Image* resizeImage = alloc_image("resize", size.height, size.width);

	for(unsigned c = 0; c < resizeImage->height; c++)
		for(unsigned r = 0; r < resizeImage->width; r++){
			PixelPoint pixelPoint = (PixelPoint){ (double)r / resizeImage->width, (double)c / resizeImage->height };
			// PixelPoint pixelPoint = (PixelPoint){ 0, 0 };
			*(resizeImage->data + (c * resizeImage->width) + r) = pixelPointColor(pixelPoint, refImage);
		}

	/* for(unsigned p = 0; p < resizeImage->width * resizeImage->height; p++){} */

	return resizeImage;
}

Rasteron_Image* cropImgOp(ref_image_t refImage, enum CROP_Type type, double factor){
	assert(refImage != NULL);
	if(type == CROP_None || factor <= 0.0) return copyImgOp(refImage);

	if(factor > 1.0) factor = 1.0 / factor; // ensure that crop factor is less than 1

	unsigned height = (type != CROP_Bottom && type != CROP_Top)? refImage->height : (unsigned)(refImage->height * factor);
	unsigned width = (type != CROP_Left && type != CROP_Right)? refImage->width : (unsigned)(refImage->width * factor);

	Rasteron_Image* cropImage = alloc_image("crop", height, width);

	unsigned offset;
	if(type == CROP_Left) offset = refImage->width - width;
	else if(type == CROP_Top) offset = (refImage->height - height) * width;
	else offset = 0;

	for(unsigned c = 0; c < height; c++){
		for(unsigned r = 0; r < width; r++){
			*(cropImage->data + (r + (c * width))) = *(refImage->data + offset); //  copy from source to destination
			offset++;
		}
		if(type == CROP_Left || type == CROP_Right) offset += refImage->width - width; // skip to next row position
	}

	return cropImage;
}

Rasteron_Image* mirrorImgOp(ref_image_t refImage){
	assert(refImage != NULL);

	Rasteron_Image* mirrorImage = copyImgOp(refImage);
	mirrorImage->name = "mirror";

	for (unsigned r = 0; r < refImage->height; r++) // Mirror Function
		for (unsigned c = 0; c < refImage->width; c++) {
			*(mirrorImage->data + (r * refImage->width) + c) = *(refImage->data + ((r + 1) * refImage->width) - c - 1);
			*(mirrorImage->data + ((r + 1) * refImage->width) - c - 1) = *(refImage->data + (r * refImage->width) + c); // mirror right pixel
		}
	
	return mirrorImage;
}

Rasteron_Image* flipImgOp(ref_image_t refImage, enum FLIP_Type type){
	assert(refImage != NULL);
	if(type == FLIP_None) return copyImgOp(refImage);
	
	Rasteron_Image* flipImg = NULL;
	if(type == FLIP_Upside){
		flipImg = alloc_image("flip", refImage->height, refImage->width); // parameters srcOffset source
		for(unsigned p = 0; p < refImage->height * refImage->width; p++)
			*(flipImg->data + (refImage->height * refImage->width) - p - 1) = *(refImage->data + p); // copies pixels in reverse
	} else if(type == FLIP_Clock) {
		flipImg = alloc_image("flip", refImage->width, refImage->height); // parameters inverse of source
		unsigned dstOffset = 0;

		for(unsigned w = 0; w < ((!_invertImage)? refImage->width : refImage->height); w++){
			unsigned srcOffset = (refImage->width * refImage->height) + w;
			for(unsigned h = 0; h < ((!_invertImage)? refImage->height : refImage->width); h++){
				srcOffset -= ((!_invertImage)? refImage->width : refImage->height);
				*(flipImg->data + dstOffset) = *(refImage->data + srcOffset);
				dstOffset++;
			}
		}
	} else if(type == FLIP_Counter){
		Rasteron_Image* flipUpsideImg = flipImgOp(refImage, FLIP_Upside);
		flipImg = flipImgOp(flipUpsideImg, FLIP_Clock);
		dealloc_image(flipUpsideImg);
	}

	return flipImg;
}