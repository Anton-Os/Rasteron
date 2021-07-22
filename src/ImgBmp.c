#include "Rasteron.h"

#ifdef USE_IMG_BMP

void loadImage_BMP(const char* fileName, Image* image) {
	image->fileFormat = IMG_Bmp;

	FILE* bmpFile;
	#ifdef _WIN32
		errno_t err = fopen_s(&bmpFile, fileName, "rb");
		if (err) {
			printf("Error opening %s fopen_s error code %d", fileName, err);
			return;
		}
	#else
		if ((bmpFile = fopen(fileName, "rb") == NULL)) {
			printf("Error opening %s", fileName);
			return;
		}
	#endif // _WIN32


	fread(&image->imageData.bmp.typeCheck, sizeof(uint16_t), 1, bmpFile);
	if (image->imageData.bmp.typeCheck != 0x4D42) {
		printf("%s is not a valid BMP file", fileName);
		return;
	}

	// https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/
	// I used the link above to calculate the positions of each of the data members

	fseek(bmpFile, 10, SEEK_SET);
	fread(&image->imageData.bmp.offset, sizeof(uint32_t), 1, bmpFile);

	fseek(bmpFile, 18, SEEK_SET);
	fread(&image->imageData.bmp.width, sizeof(int32_t), 1, bmpFile);
	fseek(bmpFile, 22, SEEK_SET);
	fread(&image->imageData.bmp.height, sizeof(int32_t), 1, bmpFile);

	// Data Reading Time!!!

	image->imageData.bmp.data = (uint32_t*)malloc(abs(image->imageData.bmp.width) * abs(image->imageData.bmp.height) * (uint32_t)sizeof(uint32_t));
	fseek(bmpFile, image->imageData.bmp.offset, SEEK_SET);
	fread((uint32_t*)image->imageData.bmp.data, sizeof(uint32_t), abs(image->imageData.bmp.width) * abs(image->imageData.bmp.height), bmpFile);


	fclose(bmpFile);
	return;
}

void writeImage_BMP(const char* fileName, Rasteron_Image* image){
	// TODO: Implement Writing Code
}

void delImage_BMP(Image* image) {
	if (image->fileFormat != IMG_Bmp) {
		puts("Image provided for deletion is not BMP type!");
		return;
	}
	free(image->imageData.bmp.data);
	image->fileFormat = IMG_NonValid;
	return;
}

#endif
