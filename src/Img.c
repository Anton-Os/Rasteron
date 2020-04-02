#include "ImageLoader.h"

// Reverses the blue and the red bits of the image
void revrsColorEncoding(uint32_t* raster, unsigned int pCount) {
	unsigned int bCount = pCount * 8;
	for (unsigned int i = 0; i < pCount; i++) {
		unsigned int val = *(raster + i);
		unsigned int res = ((val & 0xFF) << 16) + (val & 0xFF00) + ((val >>  16) & 0xFF);
		*(raster + i) = res;
	}
	return;
}

void makeSolidColor(uint32_t* raster, unsigned int pCount, uint32_t colorVal) {
	for (unsigned int i = 0; i < pCount; i++)
		*(raster + i) = colorVal;
	return;
}

void changeSolidColor(uint32_t* raster, unsigned int pCount, uint32_t newColorVal, uint32_t oldColorVal) {
	for (unsigned int i = 0; i < pCount; i++)
		if (*(raster + i) == oldColorVal)
			*(raster + i) = newColorVal;
}


/* static enum IMG_FileFormat compFileExtension(const char* fileName){
	return IMG_NonValid;
}

void createImage(const char* fileName, Image* image){
	enum fileFormat = compFileExtension(fileName);
} */