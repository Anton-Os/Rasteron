// --------------------------------   Support Functions   -------------------------------- //

#include "support_def.h"

uint32_t color_unique(){
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

uint32_t color_level(uint32_t color, double level){
	if(level >= 1.0) return 0xFFFFFFFF;
	else if(level <= 0.0) return 0xFF000000;
	else if(level == 0.5) return color;
	else if(level > 0.5) return colors_blend(color, 0xFFFFFFFF, (level - 0.5) * 2.0);
	else if(level < 0.5) return colors_blend(color, 0xFF000000, (0.5 - level) * 2.0);
	else return color;
}

uint32_t color_invert(uint32_t refColor){
	uint8_t alpha = (refColor & ALPHA_CHANNEL) >> 24;
	uint8_t red = 0xFF - ((refColor & RED_CHANNEL) >> 16);
	uint8_t green = 0xFF - ((refColor & GREEN_CHANNEL) >> 8);
	uint8_t blue = 0xFF - (refColor & BLUE_CHANNEL);

	uint32_t result = ((alpha << 24) | (red << 16) | (green << 8) | blue);
	return result;
}


// --------------------------------   Basic Operations   -------------------------------- //

#include "Rasteron.h"

Rasteron_Image* loadImgOp(const char* fileName){
	FileImage fileImage;

	loadFromFile(fileName, &fileImage);
	if(fileImage.fileFormat == IMG_NonValid) return errorImgOp("Invalid image file");

    Rasteron_Image* refImage = NULL;
    switch(fileImage.fileFormat){
#ifdef USE_IMG_TIFF
	case(IMG_Tiff):
		refImage = RASTERON_ALLOC("tiff", fileImage.data.tiff.length, fileImage.data.tiff.width);
		bitSwitch_RB(fileImage.data.tiff.data, fileImage.data.tiff.width * fileImage.data.tiff.length);
		for(unsigned i = 0; i < refImage->width * refImage->height; i++)
		   // *(refImage->data + (refImage->width * refImage->height) - 1 - i) = *(fileImage.data.tiff.data + i); // copying operation
		   *(refImage->data + i) = *(fileImage.data.tiff.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_BMP
	case(IMG_Bmp):
		refImage = RASTERON_ALLOC("bmp", abs(fileImage.data.bmp.height), abs(fileImage.data.bmp.width));
		for (unsigned i = 0; i < refImage->width * refImage->height; i++)
			// *(refImage->data + (refImage->width * refImage->height) - 1 - i) = *(fileImage.data.bmp.data + i); // copying operation
			*(refImage->data + i) = *(fileImage.data.bmp.data + i); // copying operation
		break;
#endif
#ifdef USE_IMG_PNG
	case(IMG_Png):
		refImage = RASTERON_ALLOC("png", fileImage.data.png.height, fileImage.data.png.width);
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
	Rasteron_Image* solidImage = RASTERON_ALLOC("solid", size.height, size.width);

	for (unsigned i = 0; i < solidImage->height * solidImage->width; i++)
		*(solidImage->data + i) = color;

	return solidImage;
}

Rasteron_Image* copyImgOp(ref_image_t refImage){
	if(refImage == NULL) return errorImgOp("Invalid ref for copyImgOp()");

	Rasteron_Image* copyImage = RASTERON_ALLOC("copy", refImage->height, refImage->width);

	for(unsigned p = 0; p < refImage->width * refImage->height; p++)
		*(copyImage->data + p) = *(refImage->data + p); // copy each pixel

	return copyImage;
}

Rasteron_Image* resizeImgOp(ImageSize size, ref_image_t refImage){
	if(refImage == NULL) return errorImgOp("Invalid ref for resizeImgOp()");

	Rasteron_Image* resizeImage = RASTERON_ALLOC("resize", size.height, size.width);

	for(unsigned c = 0; c < resizeImage->height; c++)
		for(unsigned r = 0; r < resizeImage->width; r++){
			PixelPoint pixelPoint = (PixelPoint){ (double)r / resizeImage->width, (double)c / resizeImage->height };
			// PixelPoint pixelPoint = (PixelPoint){ 0, 0 };
			*(resizeImage->data + (c * resizeImage->width) + r) = pixPoint_color(pixelPoint, refImage);
		}

	/* for(unsigned p = 0; p < resizeImage->width * resizeImage->height; p++){} */

	return resizeImage;
}

Rasteron_Image* cropImgOp(ref_image_t refImage, enum SIDE_Type type, double factor){
	if(refImage == NULL) return errorImgOp("Invalid ref for copyImgOp()");
	if(type == SIDE_None || type == SIDE_Radial || factor <= 0.0) return copyImgOp(refImage);

	if(factor > 1.0) factor = 1.0 / factor; // ensure that crop factor is less than 1

	unsigned height = (type != SIDE_Bottom && type != SIDE_Top)? refImage->height : (unsigned)(refImage->height * factor);
	unsigned width = (type != SIDE_Left && type != SIDE_Right)? refImage->width : (unsigned)(refImage->width * factor);

	Rasteron_Image* cropImage = RASTERON_ALLOC("crop", height, width);

	unsigned offset;
	if(type == SIDE_Left) offset = refImage->width - width;
	else if(type == SIDE_Top) offset = (refImage->height - height) * width;
	else offset = 0;

	for(unsigned c = 0; c < height; c++){
		for(unsigned r = 0; r < width; r++){
			*(cropImage->data + (r + (c * width))) = *(refImage->data + offset); //  copy from source to destination
			offset++;
		}
		if(type == SIDE_Left || type == SIDE_Right) offset += refImage->width - width; // skip to next row position
	}

	return cropImage;
}

Rasteron_Image* mirrorImgOp(ref_image_t refImage){
	if(refImage == NULL) return errorImgOp("Invalid ref for mirrorImgOp()");

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
	if(refImage == NULL) return errorImgOp("Invalid ref for flipImgOp()");
	if(type == FLIP_None) return copyImgOp(refImage);
	
	Rasteron_Image* flipImg = NULL;
	if(type == FLIP_Upside){
		flipImg = RASTERON_ALLOC("flip", refImage->height, refImage->width); // parameters srcOffset source
		for(unsigned p = 0; p < refImage->height * refImage->width; p++)
			*(flipImg->data + (refImage->height * refImage->width) - p - 1) = *(refImage->data + p); // copies pixels in reverse
	} else if(type == FLIP_Clock) {
		flipImg = RASTERON_ALLOC("flip", refImage->width, refImage->height); // parameters inverse of source
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
		RASTERON_DEALLOC(flipUpsideImg);
	}

	return flipImg;
}

Rasteron_Image* errorImgOp(const char* errorMsg){
	fprintf(stderr, "ERROR! message: %s", errorMsg);

	char errorFilePath[1024];
    genFullFilePath("Error.bmp", errorFilePath);

 	return loadImgOp(errorFilePath);
}
